

/**************************************************************************************************
 *** This file was autogenerated from GrConditionalInUniform.fp; do not modify.
 **************************************************************************************************/
#include "GrConditionalInUniform.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLConditionalInUniform : public GrGLSLFragmentProcessor {
public:
    GrGLSLConditionalInUniform() {}
    void emitCode(EmitArgs& args) override {
        GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
        const GrConditionalInUniform& _outer = args.fFp.cast<GrConditionalInUniform>();
        (void) _outer;
        auto test = _outer.test;
        (void) test;
        auto color = _outer.color;
        (void) color;
        if (test) {
            colorVar = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag, kHalf4_GrSLType, "color");
        }
        fragBuilder->codeAppendf(
R"SkSL(if (%s) {
    return %s;
} else {
    return half4(1.0);
}
)SkSL"
, (_outer.test ? "true" : "false"), colorVar.isValid() ? args.fUniformHandler->getUniformCStr(colorVar) : "half4(0)");
    }
private:
    void onSetData(const GrGLSLProgramDataManager& pdman, const GrFragmentProcessor& _proc) override {
        const GrConditionalInUniform& _outer = _proc.cast<GrConditionalInUniform>();
        {
        if (colorVar.isValid()) {
            const SkPMColor4f& colorValue = _outer.color;
            if (colorPrev != colorValue) {
                colorPrev = colorValue;
                pdman.set4fv(colorVar, 1, colorValue.vec());
            }
        }
        }
    }
SkPMColor4f colorPrev = {SK_FloatNaN, SK_FloatNaN, SK_FloatNaN, SK_FloatNaN};
    UniformHandle colorVar;
};
std::unique_ptr<GrGLSLFragmentProcessor> GrConditionalInUniform::onMakeProgramImpl() const {
    return std::make_unique<GrGLSLConditionalInUniform>();
}
void GrConditionalInUniform::onGetGLSLProcessorKey(const GrShaderCaps& caps, GrProcessorKeyBuilder* b) const {
    b->add32((uint32_t) test);
}
bool GrConditionalInUniform::onIsEqual(const GrFragmentProcessor& other) const {
    const GrConditionalInUniform& that = other.cast<GrConditionalInUniform>();
    (void) that;
    if (test != that.test) return false;
    if (color != that.color) return false;
    return true;
}
GrConditionalInUniform::GrConditionalInUniform(const GrConditionalInUniform& src)
: INHERITED(kGrConditionalInUniform_ClassID, src.optimizationFlags())
, test(src.test)
, color(src.color) {
        this->cloneAndRegisterAllChildProcessors(src);
}
std::unique_ptr<GrFragmentProcessor> GrConditionalInUniform::clone() const {
    return std::make_unique<GrConditionalInUniform>(*this);
}
#if GR_TEST_UTILS
SkString GrConditionalInUniform::onDumpInfo() const {
    return SkStringPrintf("(test=%s, color=half4(%f, %f, %f, %f))", (test ? "true" : "false"), color.fR, color.fG, color.fB, color.fA);
}
#endif
