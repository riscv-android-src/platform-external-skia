

/**************************************************************************************************
 *** This file was autogenerated from GrChildProcessorSampleMatrixSingleUniform.fp; do not modify.
 **************************************************************************************************/
#ifndef GrChildProcessorSampleMatrixSingleUniform_DEFINED
#define GrChildProcessorSampleMatrixSingleUniform_DEFINED

#include "include/core/SkM44.h"
#include "include/core/SkTypes.h"


#include "src/gpu/GrFragmentProcessor.h"

class GrChildProcessorSampleMatrixSingleUniform : public GrFragmentProcessor {
public:
    static std::unique_ptr<GrFragmentProcessor> Make(std::unique_ptr<GrFragmentProcessor> child) {
        return std::unique_ptr<GrFragmentProcessor>(new GrChildProcessorSampleMatrixSingleUniform(std::move(child)));
    }
    GrChildProcessorSampleMatrixSingleUniform(const GrChildProcessorSampleMatrixSingleUniform& src);
    std::unique_ptr<GrFragmentProcessor> clone() const override;
    const char* name() const override { return "ChildProcessorSampleMatrixSingleUniform"; }
private:
    GrChildProcessorSampleMatrixSingleUniform(std::unique_ptr<GrFragmentProcessor> child)
    : INHERITED(kGrChildProcessorSampleMatrixSingleUniform_ClassID, kNone_OptimizationFlags) {
        this->registerChild(std::move(child), SkSL::SampleUsage::UniformMatrix("matrix", true));    }
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
