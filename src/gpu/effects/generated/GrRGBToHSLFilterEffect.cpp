/*
 * Copyright 2019 Google LLC
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrRGBToHSLFilterEffect.fp; do not modify.
 **************************************************************************************************/
#include "GrRGBToHSLFilterEffect.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLRGBToHSLFilterEffect : public GrGLSLFragmentProcessor {
public:
    GrGLSLRGBToHSLFilterEffect() {}
    void emitCode(EmitArgs& args) override {
        GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
        const GrRGBToHSLFilterEffect& _outer = args.fFp.cast<GrRGBToHSLFilterEffect>();
        (void)_outer;
        SkString _sample1173 = this->invokeChild(0, args);
        fragBuilder->codeAppendf(
                R"SkSL(half4 c = %s;
half4 p = c.y < c.z ? half4(c.zy, -1.0, 0.66666666666666663) : half4(c.yz, 0.0, -0.33333333333333331);
half4 q = c.x < p.x ? half4(p.x, c.x, p.yw) : half4(c.x, p.x, p.yz);

half pmV = q.x;
half pmC = pmV - min(q.y, q.z);
half pmL = pmV - pmC * 0.5;
half H = abs(q.w + (q.y - q.z) / (pmC * 6.0 + 9.9999997473787516e-05));
half S = pmC / ((c.w + 9.9999997473787516e-05) - abs(pmL * 2.0 - c.w));
half L = pmL / (c.w + 9.9999997473787516e-05);
%s = half4(H, S, L, c.w);
)SkSL",
                _sample1173.c_str(), args.fOutputColor);
    }

private:
    void onSetData(const GrGLSLProgramDataManager& pdman,
                   const GrFragmentProcessor& _proc) override {}
};
GrGLSLFragmentProcessor* GrRGBToHSLFilterEffect::onCreateGLSLInstance() const {
    return new GrGLSLRGBToHSLFilterEffect();
}
void GrRGBToHSLFilterEffect::onGetGLSLProcessorKey(const GrShaderCaps& caps,
                                                   GrProcessorKeyBuilder* b) const {}
bool GrRGBToHSLFilterEffect::onIsEqual(const GrFragmentProcessor& other) const {
    const GrRGBToHSLFilterEffect& that = other.cast<GrRGBToHSLFilterEffect>();
    (void)that;
    return true;
}
GrRGBToHSLFilterEffect::GrRGBToHSLFilterEffect(const GrRGBToHSLFilterEffect& src)
        : INHERITED(kGrRGBToHSLFilterEffect_ClassID, src.optimizationFlags()) {
    this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrRGBToHSLFilterEffect::clone() const {
    return std::make_unique<GrRGBToHSLFilterEffect>(*this);
}
