/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrDualIntervalGradientColorizer.fp; do not modify.
 **************************************************************************************************/
#ifndef GrDualIntervalGradientColorizer_DEFINED
#define GrDualIntervalGradientColorizer_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"

#include "src/gpu/GrFragmentProcessor.h"

class GrDualIntervalGradientColorizer : public GrFragmentProcessor {
public:
    static std::unique_ptr<GrFragmentProcessor> Make(const SkPMColor4f& c0,
                                                     const SkPMColor4f& c1,
                                                     const SkPMColor4f& c2,
                                                     const SkPMColor4f& c3,
                                                     float threshold);
    GrDualIntervalGradientColorizer(const GrDualIntervalGradientColorizer& src);
    std::unique_ptr<GrFragmentProcessor> clone() const override;
    const char* name() const override { return "DualIntervalGradientColorizer"; }
    bool usesExplicitReturn() const override;
    SkPMColor4f scale01;
    SkPMColor4f bias01;
    SkPMColor4f scale23;
    SkPMColor4f bias23;
    float threshold;

private:
    GrDualIntervalGradientColorizer(SkPMColor4f scale01,
                                    SkPMColor4f bias01,
                                    SkPMColor4f scale23,
                                    SkPMColor4f bias23,
                                    float threshold)
            : INHERITED(kGrDualIntervalGradientColorizer_ClassID, kNone_OptimizationFlags)
            , scale01(scale01)
            , bias01(bias01)
            , scale23(scale23)
            , bias23(bias23)
            , threshold(threshold) {
        this->setUsesSampleCoordsDirectly();
    }
    GrGLSLFragmentProcessor* onCreateGLSLInstance() const override;
    void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
    bool onIsEqual(const GrFragmentProcessor&) const override;
#if GR_TEST_UTILS
    SkString onDumpInfo() const override;
#endif
    GR_DECLARE_FRAGMENT_PROCESSOR_TEST
    typedef GrFragmentProcessor INHERITED;
};
#endif
