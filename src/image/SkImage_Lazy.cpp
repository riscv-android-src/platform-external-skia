/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/image/SkImage_Lazy.h"

#include "include/core/SkBitmap.h"
#include "include/core/SkData.h"
#include "include/core/SkImageGenerator.h"
#include "src/core/SkBitmapCache.h"
#include "src/core/SkCachedData.h"
#include "src/core/SkImagePriv.h"
#include "src/core/SkNextID.h"

#if SK_SUPPORT_GPU
#include "include/core/SkYUVAIndex.h"
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/GrRecordingContext.h"
#include "include/private/GrResourceKey.h"
#include "src/core/SkResourceCache.h"
#include "src/core/SkYUVPlanesCache.h"
#include "src/gpu/GrBitmapTextureMaker.h"
#include "src/gpu/GrCaps.h"
#include "src/gpu/GrColorSpaceXform.h"
#include "src/gpu/GrGpuResourcePriv.h"
#include "src/gpu/GrImageTextureMaker.h"
#include "src/gpu/GrPaint.h"
#include "src/gpu/GrProxyProvider.h"
#include "src/gpu/GrRecordingContextPriv.h"
#include "src/gpu/GrRenderTargetContext.h"
#include "src/gpu/GrSamplerState.h"
#include "src/gpu/SkGr.h"
#include "src/gpu/effects/GrYUVtoRGBEffect.h"
#endif

// Ref-counted tuple(SkImageGenerator, SkMutex) which allows sharing one generator among N images
class SharedGenerator final : public SkNVRefCnt<SharedGenerator> {
public:
    static sk_sp<SharedGenerator> Make(std::unique_ptr<SkImageGenerator> gen) {
        return gen ? sk_sp<SharedGenerator>(new SharedGenerator(std::move(gen))) : nullptr;
    }

    // This is thread safe.  It is a const field set in the constructor.
    const SkImageInfo& getInfo() { return fGenerator->getInfo(); }

private:
    explicit SharedGenerator(std::unique_ptr<SkImageGenerator> gen)
            : fGenerator(std::move(gen)) {
        SkASSERT(fGenerator);
    }

    friend class ScopedGenerator;
    friend class SkImage_Lazy;

    std::unique_ptr<SkImageGenerator> fGenerator;
    SkMutex                           fMutex;
};

///////////////////////////////////////////////////////////////////////////////

SkImage_Lazy::Validator::Validator(sk_sp<SharedGenerator> gen, const SkIRect* subset,
                                   const SkColorType* colorType, sk_sp<SkColorSpace> colorSpace)
        : fSharedGenerator(std::move(gen)) {
    if (!fSharedGenerator) {
        return;
    }

    // The following generator accessors are safe without acquiring the mutex (const getters).
    // TODO: refactor to use a ScopedGenerator instead, for clarity.
    const SkImageInfo& info = fSharedGenerator->fGenerator->getInfo();
    if (info.isEmpty()) {
        fSharedGenerator.reset();
        return;
    }

    fUniqueID = fSharedGenerator->fGenerator->uniqueID();
    const SkIRect bounds = SkIRect::MakeWH(info.width(), info.height());
    if (subset) {
        if (!bounds.contains(*subset)) {
            fSharedGenerator.reset();
            return;
        }
        if (*subset != bounds) {
            // we need a different uniqueID since we really are a subset of the raw generator
            fUniqueID = SkNextID::ImageID();
        }
    } else {
        subset = &bounds;
    }

    fInfo   = info.makeDimensions(subset->size());
    fOrigin = SkIPoint::Make(subset->x(), subset->y());
    if (colorType || colorSpace) {
        if (colorType) {
            fInfo = fInfo.makeColorType(*colorType);
        }
        if (colorSpace) {
            fInfo = fInfo.makeColorSpace(colorSpace);
        }
        fUniqueID = SkNextID::ImageID();
    }
}

///////////////////////////////////////////////////////////////////////////////

// Helper for exclusive access to a shared generator.
class SkImage_Lazy::ScopedGenerator {
public:
    ScopedGenerator(const sk_sp<SharedGenerator>& gen)
      : fSharedGenerator(gen)
      , fAutoAquire(gen->fMutex) {}

    SkImageGenerator* operator->() const {
        fSharedGenerator->fMutex.assertHeld();
        return fSharedGenerator->fGenerator.get();
    }

    operator SkImageGenerator*() const {
        fSharedGenerator->fMutex.assertHeld();
        return fSharedGenerator->fGenerator.get();
    }

private:
    const sk_sp<SharedGenerator>& fSharedGenerator;
    SkAutoMutexExclusive          fAutoAquire;
};

///////////////////////////////////////////////////////////////////////////////

SkImage_Lazy::SkImage_Lazy(Validator* validator)
        : INHERITED(validator->fInfo, validator->fUniqueID)
        , fSharedGenerator(std::move(validator->fSharedGenerator))
        , fOrigin(validator->fOrigin) {
    SkASSERT(fSharedGenerator);
}


//////////////////////////////////////////////////////////////////////////////////////////////////

static bool generate_pixels(SkImageGenerator* gen, const SkPixmap& pmap, int originX, int originY) {
    const int genW = gen->getInfo().width();
    const int genH = gen->getInfo().height();
    const SkIRect srcR = SkIRect::MakeWH(genW, genH);
    const SkIRect dstR = SkIRect::MakeXYWH(originX, originY, pmap.width(), pmap.height());
    if (!srcR.contains(dstR)) {
        return false;
    }

    // If they are requesting a subset, we have to have a temp allocation for full image, and
    // then copy the subset into their allocation
    SkBitmap full;
    SkPixmap fullPM;
    const SkPixmap* dstPM = &pmap;
    if (srcR != dstR) {
        if (!full.tryAllocPixels(pmap.info().makeWH(genW, genH))) {
            return false;
        }
        if (!full.peekPixels(&fullPM)) {
            return false;
        }
        dstPM = &fullPM;
    }

    if (!gen->getPixels(dstPM->info(), dstPM->writable_addr(), dstPM->rowBytes())) {
        return false;
    }

    if (srcR != dstR) {
        if (!full.readPixels(pmap, originX, originY)) {
            return false;
        }
    }
    return true;
}

bool SkImage_Lazy::getROPixels(SkBitmap* bitmap, SkImage::CachingHint chint) const {
    auto check_output_bitmap = [bitmap]() {
        SkASSERT(bitmap->isImmutable());
        SkASSERT(bitmap->getPixels());
        (void)bitmap;
    };

    auto desc = SkBitmapCacheDesc::Make(this);
    if (SkBitmapCache::Find(desc, bitmap)) {
        check_output_bitmap();
        return true;
    }

    if (SkImage::kAllow_CachingHint == chint) {
        SkPixmap pmap;
        SkBitmapCache::RecPtr cacheRec = SkBitmapCache::Alloc(desc, this->imageInfo(), &pmap);
        if (!cacheRec ||
            !generate_pixels(ScopedGenerator(fSharedGenerator), pmap,
                             fOrigin.x(), fOrigin.y())) {
            return false;
        }
        SkBitmapCache::Add(std::move(cacheRec), bitmap);
        this->notifyAddedToRasterCache();
    } else {
        if (!bitmap->tryAllocPixels(this->imageInfo()) ||
            !generate_pixels(ScopedGenerator(fSharedGenerator), bitmap->pixmap(), fOrigin.x(),
                             fOrigin.y())) {
            return false;
        }
        bitmap->setImmutable();
    }

    check_output_bitmap();
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

bool SkImage_Lazy::onReadPixels(const SkImageInfo& dstInfo, void* dstPixels, size_t dstRB,
                                int srcX, int srcY, CachingHint chint) const {
    SkBitmap bm;
    if (this->getROPixels(&bm, chint)) {
        return bm.readPixels(dstInfo, dstPixels, dstRB, srcX, srcY);
    }
    return false;
}

sk_sp<SkData> SkImage_Lazy::onRefEncoded() const {
    ScopedGenerator generator(fSharedGenerator);
    return generator->refEncodedData();
}

bool SkImage_Lazy::onIsValid(GrRecordingContext* context) const {
    ScopedGenerator generator(fSharedGenerator);
    return generator->isValid(context);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#if SK_SUPPORT_GPU
GrSurfaceProxyView SkImage_Lazy::refView(GrRecordingContext* context, GrMipmapped mipMapped) const {
    if (!context) {
        return {};
    }

    GrImageTextureMaker textureMaker(context, this, GrImageTexGenPolicy::kDraw);
    return textureMaker.view(mipMapped);
}
#endif

sk_sp<SkImage> SkImage_Lazy::onMakeSubset(const SkIRect& subset, GrDirectContext*) const {
    SkASSERT(this->bounds().contains(subset));
    SkASSERT(this->bounds() != subset);

    const SkIRect generatorSubset = subset.makeOffset(fOrigin);
    const SkColorType colorType = this->colorType();
    Validator validator(fSharedGenerator, &generatorSubset, &colorType, this->refColorSpace());
    return validator ? sk_sp<SkImage>(new SkImage_Lazy(&validator)) : nullptr;
}

sk_sp<SkImage> SkImage_Lazy::onMakeColorTypeAndColorSpace(SkColorType targetCT,
                                                          sk_sp<SkColorSpace> targetCS,
                                                          GrDirectContext*) const {
    SkAutoMutexExclusive autoAquire(fOnMakeColorTypeAndSpaceMutex);
    if (fOnMakeColorTypeAndSpaceResult &&
        targetCT == fOnMakeColorTypeAndSpaceResult->colorType() &&
        SkColorSpace::Equals(targetCS.get(), fOnMakeColorTypeAndSpaceResult->colorSpace())) {
        return fOnMakeColorTypeAndSpaceResult;
    }
    const SkIRect generatorSubset =
            SkIRect::MakeXYWH(fOrigin.x(), fOrigin.y(), this->width(), this->height());
    Validator validator(fSharedGenerator, &generatorSubset, &targetCT, targetCS);
    sk_sp<SkImage> result = validator ? sk_sp<SkImage>(new SkImage_Lazy(&validator)) : nullptr;
    if (result) {
        fOnMakeColorTypeAndSpaceResult = result;
    }
    return result;
}

sk_sp<SkImage> SkImage_Lazy::onReinterpretColorSpace(sk_sp<SkColorSpace> newCS) const {
    // TODO: The correct thing is to clone the generator, and modify its color space. That's hard,
    // because we don't have a clone method, and generator is public (and derived-from by clients).
    // So do the simple/inefficient thing here, and fallback to raster when this is called.

    // We allocate the bitmap with the new color space, then generate the image using the original.
    SkBitmap bitmap;
    if (bitmap.tryAllocPixels(this->imageInfo().makeColorSpace(std::move(newCS)))) {
        SkPixmap pixmap = bitmap.pixmap();
        pixmap.setColorSpace(this->refColorSpace());
        if (generate_pixels(ScopedGenerator(fSharedGenerator), pixmap, fOrigin.x(), fOrigin.y())) {
            bitmap.setImmutable();
            return SkImage::MakeFromBitmap(bitmap);
        }
    }
    return nullptr;
}

sk_sp<SkImage> SkImage::MakeFromGenerator(std::unique_ptr<SkImageGenerator> generator) {
    SkImage_Lazy::Validator
            validator(SharedGenerator::Make(std::move(generator)), nullptr, nullptr, nullptr);

    return validator ? sk_make_sp<SkImage_Lazy>(&validator) : nullptr;
}

sk_sp<SkImage> SkImage::DecodeToRaster(const void* encoded, size_t length, const SkIRect* subset) {
    // The generator will not outlive this function, so we can wrap the encoded data without copy
    auto gen = SkImageGenerator::MakeFromEncoded(SkData::MakeWithoutCopy(encoded, length));
    if (!gen) {
        return nullptr;
    }
    SkImageInfo info = gen->getInfo();
    if (info.isEmpty()) {
        return nullptr;
    }

    SkIPoint origin = {0, 0};
    if (subset) {
        if (!SkIRect::MakeWH(info.width(), info.height()).contains(*subset)) {
            return nullptr;
        }
        info = info.makeDimensions(subset->size());
        origin = {subset->x(), subset->y()};
    }

    size_t rb = info.minRowBytes();
    if (rb == 0) {
        return nullptr; // rb was too big
    }
    size_t size = info.computeByteSize(rb);
    if (size == SIZE_MAX) {
        return nullptr;
    }
    auto data = SkData::MakeUninitialized(size);

    SkPixmap pmap(info, data->writable_data(), rb);
    if (!generate_pixels(gen.get(), pmap, origin.x(), origin.y())) {
        return nullptr;
    }

    return SkImage::MakeRasterData(info, data, rb);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

#if SK_SUPPORT_GPU

GrSurfaceProxyView SkImage_Lazy::textureProxyViewFromPlanes(GrRecordingContext* ctx,
                                                            SkBudgeted budgeted) const {
    SkYUVASizeInfo yuvSizeInfo;
    SkYUVAIndex yuvaIndices[SkYUVAIndex::kIndexCount];
    SkYUVColorSpace yuvColorSpace;
    const void* planes[SkYUVASizeInfo::kMaxCount];

    sk_sp<SkCachedData> dataStorage =
            this->getPlanes(&yuvSizeInfo, yuvaIndices, &yuvColorSpace, planes);
    if (!dataStorage) {
        return {};
    }

    GrSurfaceProxyView yuvViews[SkYUVASizeInfo::kMaxCount];
    for (int i = 0; i < SkYUVASizeInfo::kMaxCount; ++i) {
        if (yuvSizeInfo.fSizes[i].isEmpty()) {
            SkASSERT(!yuvSizeInfo.fWidthBytes[i]);
            continue;
        }

        int componentWidth = yuvSizeInfo.fSizes[i].fWidth;
        int componentHeight = yuvSizeInfo.fSizes[i].fHeight;
        // If the sizes of the components are not all the same we choose to create exact-match
        // textures for the smaller ones rather than add a texture domain to the draw.
        // TODO: revisit this decision to improve texture reuse?
        SkBackingFit fit =
                (componentWidth  != yuvSizeInfo.fSizes[0].fWidth) ||
                (componentHeight != yuvSizeInfo.fSizes[0].fHeight)
                ? SkBackingFit::kExact : SkBackingFit::kApprox;

        SkImageInfo imageInfo = SkImageInfo::MakeA8(componentWidth, componentHeight);
        SkCachedData* dataStoragePtr = dataStorage.get();
        // We grab a ref to cached yuv data. When the SkBitmap we create below goes away it will
        // call the YUVGen_DataReleaseProc which will release this ref.
        // DDL TODO: Currently we end up creating a lazy proxy that will hold onto a ref to the
        // SkImage in its lambda. This means that we'll keep the ref on the YUV data around for the
        // life time of the proxy and not just upload. For non-DDL draws we should look into
        // releasing this SkImage after uploads (by deleting the lambda after instantiation).
        dataStoragePtr->ref();
        SkBitmap bitmap;
        auto releaseProc = [](void*, void* data) {
            SkCachedData* cachedData = static_cast<SkCachedData*>(data);
            SkASSERT(cachedData);
            cachedData->unref();
        };

        SkAssertResult(bitmap.installPixels(imageInfo, const_cast<void*>(planes[i]),
                                            yuvSizeInfo.fWidthBytes[i], releaseProc,
                                            dataStoragePtr));
        bitmap.setImmutable();

        GrBitmapTextureMaker maker(ctx, bitmap, fit);
        yuvViews[i] = maker.view(GrMipmapped::kNo);

        if (!yuvViews[i]) {
            return {};
        }

        SkASSERT(yuvViews[i].proxy()->dimensions() == yuvSizeInfo.fSizes[i]);
    }

    // TODO: investigate preallocating mip maps here
    GrColorType ct = SkColorTypeToGrColorType(this->colorType());
    auto renderTargetContext = GrRenderTargetContext::Make(
            ctx, ct, nullptr, SkBackingFit::kExact, this->dimensions(), 1, GrMipmapped::kNo,
            GrProtected::kNo, kTopLeft_GrSurfaceOrigin, budgeted);
    if (!renderTargetContext) {
        return {};
    }

    GrPaint paint;
    const auto& caps = *ctx->priv().caps();
    std::unique_ptr<GrFragmentProcessor> yuvToRgbProcessor = GrYUVtoRGBEffect::Make(
            yuvViews, yuvaIndices, yuvColorSpace, GrSamplerState::Filter::kNearest, caps);

    // The pixels after yuv->rgb will be in the generator's color space.
    // If onMakeColorTypeAndColorSpace has been called then this will not match this image's
    // color space. To correct this, apply a color space conversion from the generator's color
    // space to this image's color space.
    SkColorSpace* srcColorSpace;
    {
        ScopedGenerator generator(fSharedGenerator);
        srcColorSpace = generator->getInfo().colorSpace();
    }
    SkColorSpace* dstColorSpace = this->colorSpace();

    // If the caller expects the pixels in a different color space than the one from the image,
    // apply a color conversion to do this.
    std::unique_ptr<GrFragmentProcessor> colorConversionProcessor =
            GrColorSpaceXformEffect::Make(std::move(yuvToRgbProcessor),
                                          srcColorSpace, kOpaque_SkAlphaType,
                                          dstColorSpace, kOpaque_SkAlphaType);
    paint.setColorFragmentProcessor(std::move(colorConversionProcessor));

    paint.setPorterDuffXPFactory(SkBlendMode::kSrc);
    const SkRect r = SkRect::MakeIWH(yuvSizeInfo.fSizes[0].fWidth, yuvSizeInfo.fSizes[0].fHeight);

    SkMatrix m = SkEncodedOriginToMatrix(yuvSizeInfo.fOrigin, r.width(), r.height());
    renderTargetContext->drawRect(nullptr, std::move(paint), GrAA::kNo, m, r);

    SkASSERT(renderTargetContext->asTextureProxy());
    return renderTargetContext->readSurfaceView();
}

sk_sp<SkCachedData> SkImage_Lazy::getPlanes(
        SkYUVASizeInfo* yuvaSizeInfo,
        SkYUVAIndex yuvaIndices[SkYUVAIndex::kIndexCount],
        SkYUVColorSpace* yuvColorSpace,
        const void* outPlanes[SkYUVASizeInfo::kMaxCount]) const {
    ScopedGenerator generator(fSharedGenerator);

    sk_sp<SkCachedData> data;
    SkYUVPlanesCache::Info yuvInfo;
    data.reset(SkYUVPlanesCache::FindAndRef(generator->uniqueID(), &yuvInfo));

    void* planes[SkYUVASizeInfo::kMaxCount];

    if (data.get()) {
        planes[0] = (void*)data->data();  // we should always have at least one plane

        for (int i = 1; i < SkYUVASizeInfo::kMaxCount; ++i) {
            if (!yuvInfo.fSizeInfo.fWidthBytes[i]) {
                SkASSERT(!yuvInfo.fSizeInfo.fWidthBytes[i] && !yuvInfo.fSizeInfo.fSizes[i].fHeight);
                planes[i] = nullptr;
                continue;
            }

            planes[i] = (uint8_t*)planes[i - 1] + (yuvInfo.fSizeInfo.fWidthBytes[i - 1] *
                                                   yuvInfo.fSizeInfo.fSizes[i - 1].fHeight);
        }
    } else {
        // Fetch yuv plane sizes for memory allocation.
        if (!generator->queryYUVA8(&yuvInfo.fSizeInfo, yuvInfo.fYUVAIndices,
                                   &yuvInfo.fColorSpace)) {
            return nullptr;
        }

        // Allocate the memory for YUVA
        size_t totalSize(0);
        for (int i = 0; i < SkYUVASizeInfo::kMaxCount; i++) {
            SkASSERT((yuvInfo.fSizeInfo.fWidthBytes[i] && yuvInfo.fSizeInfo.fSizes[i].fHeight) ||
                     (!yuvInfo.fSizeInfo.fWidthBytes[i] && !yuvInfo.fSizeInfo.fSizes[i].fHeight));

            totalSize += yuvInfo.fSizeInfo.fWidthBytes[i] * yuvInfo.fSizeInfo.fSizes[i].fHeight;
        }

        data.reset(SkResourceCache::NewCachedData(totalSize));

        planes[0] = data->writable_data();

        for (int i = 1; i < SkYUVASizeInfo::kMaxCount; ++i) {
            if (!yuvInfo.fSizeInfo.fWidthBytes[i]) {
                SkASSERT(!yuvInfo.fSizeInfo.fWidthBytes[i] && !yuvInfo.fSizeInfo.fSizes[i].fHeight);
                planes[i] = nullptr;
                continue;
            }

            planes[i] = (uint8_t*)planes[i-1] + (yuvInfo.fSizeInfo.fWidthBytes[i-1] *
                                                 yuvInfo.fSizeInfo.fSizes[i-1].fHeight);
        }

        // Get the YUV planes.
        if (!generator->getYUVA8Planes(yuvInfo.fSizeInfo, yuvInfo.fYUVAIndices, planes)) {
            return nullptr;
        }

        // Decoding is done, cache the resulting YUV planes
        SkYUVPlanesCache::Add(this->uniqueID(), data.get(), &yuvInfo);
    }

    *yuvaSizeInfo = yuvInfo.fSizeInfo;
    memcpy(yuvaIndices, yuvInfo.fYUVAIndices, sizeof(yuvInfo.fYUVAIndices));
    *yuvColorSpace = yuvInfo.fColorSpace;
    outPlanes[0] = planes[0];
    outPlanes[1] = planes[1];
    outPlanes[2] = planes[2];
    outPlanes[3] = planes[3];
    return data;
}

/*
 *  We have 4 ways to try to return a texture (in sorted order)
 *
 *  1. Check the cache for a pre-existing one
 *  2. Ask the generator to natively create one
 *  3. Ask the generator to return YUV planes, which the GPU can convert
 *  4. Ask the generator to return RGB(A) data, which the GPU can convert
 */
GrSurfaceProxyView SkImage_Lazy::lockTextureProxyView(GrRecordingContext* ctx,
                                                      GrImageTexGenPolicy texGenPolicy,
                                                      GrMipmapped mipMapped) const {
    // Values representing the various texture lock paths we can take. Used for logging the path
    // taken to a histogram.
    enum LockTexturePath {
        kFailure_LockTexturePath,
        kPreExisting_LockTexturePath,
        kNative_LockTexturePath,
        kCompressed_LockTexturePath, // Deprecated
        kYUV_LockTexturePath,
        kRGBA_LockTexturePath,
    };

    enum { kLockTexturePathCount = kRGBA_LockTexturePath + 1 };

    GrUniqueKey key;
    if (texGenPolicy == GrImageTexGenPolicy::kDraw) {
        GrMakeKeyFromImageID(&key, this->uniqueID(), SkIRect::MakeSize(this->dimensions()));
    }

    const GrCaps* caps = ctx->priv().caps();
    GrProxyProvider* proxyProvider = ctx->priv().proxyProvider();

    auto installKey = [&](const GrSurfaceProxyView& view) {
        SkASSERT(view && view.asTextureProxy());
        if (key.isValid()) {
            auto listener = GrMakeUniqueKeyInvalidationListener(&key, ctx->priv().contextID());
            this->addUniqueIDListener(std::move(listener));
            proxyProvider->assignUniqueKeyToProxy(key, view.asTextureProxy());
        }
    };

    auto ct = this->colorTypeOfLockTextureProxy(caps);

    // 1. Check the cache for a pre-existing one.
    if (key.isValid()) {
        auto proxy = proxyProvider->findOrCreateProxyByUniqueKey(key);
        if (proxy) {
            SK_HISTOGRAM_ENUMERATION("LockTexturePath", kPreExisting_LockTexturePath,
                                     kLockTexturePathCount);
            GrSwizzle swizzle = caps->getReadSwizzle(proxy->backendFormat(), ct);
            GrSurfaceProxyView view(std::move(proxy), kTopLeft_GrSurfaceOrigin, swizzle);
            if (mipMapped == GrMipmapped::kNo ||
                view.asTextureProxy()->mipmapped() == GrMipmapped::kYes) {
                return view;
            } else {
                // We need a mipped proxy, but we found a cached proxy that wasn't mipped. Thus we
                // generate a new mipped surface and copy the original proxy into the base layer. We
                // will then let the gpu generate the rest of the mips.
                auto mippedView = GrCopyBaseMipMapToView(ctx, view);
                if (!mippedView) {
                    // We failed to make a mipped proxy with the base copied into it. This could
                    // have been from failure to make the proxy or failure to do the copy. Thus we
                    // will fall back to just using the non mipped proxy; See skbug.com/7094.
                    return view;
                }
                proxyProvider->removeUniqueKeyFromProxy(view.asTextureProxy());
                installKey(mippedView);
                return mippedView;
            }
        }
    }

    // 2. Ask the generator to natively create one.
    {
        ScopedGenerator generator(fSharedGenerator);
        if (auto view = generator->generateTexture(ctx, this->imageInfo(), fOrigin, mipMapped,
                                                   texGenPolicy)) {
            SK_HISTOGRAM_ENUMERATION("LockTexturePath", kNative_LockTexturePath,
                                     kLockTexturePathCount);
            installKey(view);
            return view;
        }
    }

    // 3. Ask the generator to return YUV planes, which the GPU can convert. If we will be mipping
    //    the texture we skip this step so the CPU generate non-planar MIP maps for us.
    if (mipMapped == GrMipmapped::kNo && !ctx->priv().options().fDisableGpuYUVConversion) {
        // TODO: Update to create the mipped surface in the textureProxyViewFromPlanes generator and
        //  draw the base layer directly into the mipped surface.
        SkBudgeted budgeted = texGenPolicy == GrImageTexGenPolicy::kNew_Uncached_Unbudgeted
                                      ? SkBudgeted::kNo
                                      : SkBudgeted::kYes;
        auto view = this->textureProxyViewFromPlanes(ctx, budgeted);
        if (view) {
            SK_HISTOGRAM_ENUMERATION("LockTexturePath", kYUV_LockTexturePath,
                                     kLockTexturePathCount);
            installKey(view);
            return view;
        }
    }

    // 4. Ask the generator to return a bitmap, which the GPU can convert.
    auto hint = texGenPolicy == GrImageTexGenPolicy::kDraw ? CachingHint::kAllow_CachingHint
                                                           : CachingHint::kDisallow_CachingHint;
    if (SkBitmap bitmap; this->getROPixels(&bitmap, hint)) {
        // We always pass uncached here because we will cache it external to the maker based on
        // *our* cache policy. We're just using the maker to generate the texture.
        auto makerPolicy = texGenPolicy == GrImageTexGenPolicy::kNew_Uncached_Unbudgeted
                                   ? GrImageTexGenPolicy::kNew_Uncached_Unbudgeted
                                   : GrImageTexGenPolicy::kNew_Uncached_Budgeted;
        GrBitmapTextureMaker bitmapMaker(ctx, bitmap, makerPolicy);
        auto view = bitmapMaker.view(mipMapped);
        if (view) {
            installKey(view);
            SK_HISTOGRAM_ENUMERATION("LockTexturePath", kRGBA_LockTexturePath,
                                     kLockTexturePathCount);
            return view;
        }
    }

    SK_HISTOGRAM_ENUMERATION("LockTexturePath", kFailure_LockTexturePath, kLockTexturePathCount);
    return {};
}

GrColorType SkImage_Lazy::colorTypeOfLockTextureProxy(const GrCaps* caps) const {
    GrColorType ct = SkColorTypeToGrColorType(this->colorType());
    GrBackendFormat format = caps->getDefaultBackendFormat(ct, GrRenderable::kNo);
    if (!format.isValid()) {
        ct = GrColorType::kRGBA_8888;
    }
    return ct;
}

#if SK_SUPPORT_GPU
void SkImage_Lazy::addUniqueIDListener(sk_sp<SkIDChangeListener> listener) const {
    bool singleThreaded = this->unique();
    fUniqueIDListeners.add(std::move(listener), singleThreaded);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

sk_sp<SkImage> SkImage::DecodeToTexture(GrDirectContext* direct, const void* encoded,
                                        size_t length, const SkIRect* subset) {
    // img will not survive this function, so we don't need to copy/own the encoded data,
    auto img = MakeFromEncoded(SkData::MakeWithoutCopy(encoded, length));
    if (!img) {
        return nullptr;
    }
    img = img->makeTextureImage(direct);
    if (img && subset) {
        img = img->makeSubset(*subset, direct);
    }
    return img;
}
#endif  // SK_SUPPORT_GPU

#endif
