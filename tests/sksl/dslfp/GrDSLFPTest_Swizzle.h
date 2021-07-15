

/**************************************************************************************************
 *** This file was autogenerated from GrDSLFPTest_Swizzle.fp; do not modify.
 **************************************************************************************************/
#ifndef GrDSLFPTest_Swizzle_DEFINED
#define GrDSLFPTest_Swizzle_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"


#include "src/gpu/GrFragmentProcessor.h"

class GrDSLFPTest_Swizzle : public GrFragmentProcessor {
public:
    static std::unique_ptr<GrFragmentProcessor> Make() {
        return std::unique_ptr<GrFragmentProcessor>(new GrDSLFPTest_Swizzle());
    }
    GrDSLFPTest_Swizzle(const GrDSLFPTest_Swizzle& src);
    std::unique_ptr<GrFragmentProcessor> clone() const override;
    const char* name() const override { return "DSLFPTest_Swizzle"; }
private:
    GrDSLFPTest_Swizzle()
    : INHERITED(kGrDSLFPTest_Swizzle_ClassID, kNone_OptimizationFlags) {
    }
    std::unique_ptr<GrGLSLFragmentProcessor> onMakeProgramImpl() const override;
    void onGetGLSLProcessorKey(const GrShaderCaps&, GrProcessorKeyBuilder*) const override;
    bool onIsEqual(const GrFragmentProcessor&) const override;
#if GR_TEST_UTILS
    SkString onDumpInfo() const override;
#endif
    GR_DECLARE_FRAGMENT_PROCESSOR_TEST
    using INHERITED = GrFragmentProcessor;
};
#endif
