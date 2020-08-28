/*
 * Copyright 2018 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/**************************************************************************************************
 *** This file was autogenerated from GrUnrolledBinaryGradientColorizer.fp; do not modify.
 **************************************************************************************************/
#include "GrUnrolledBinaryGradientColorizer.h"

#include "src/core/SkUtils.h"
#include "src/gpu/GrTexture.h"
#include "src/gpu/glsl/GrGLSLFragmentProcessor.h"
#include "src/gpu/glsl/GrGLSLFragmentShaderBuilder.h"
#include "src/gpu/glsl/GrGLSLProgramBuilder.h"
#include "src/sksl/SkSLCPP.h"
#include "src/sksl/SkSLUtil.h"
class GrGLSLUnrolledBinaryGradientColorizer : public GrGLSLFragmentProcessor {
public:
    GrGLSLUnrolledBinaryGradientColorizer() {}
    void emitCode(EmitArgs& args) override {
        GrGLSLFPFragmentBuilder* fragBuilder = args.fFragBuilder;
        const GrUnrolledBinaryGradientColorizer& _outer =
                args.fFp.cast<GrUnrolledBinaryGradientColorizer>();
        (void)_outer;
        auto intervalCount = _outer.intervalCount;
        (void)intervalCount;
        auto scale0_1 = _outer.scale0_1;
        (void)scale0_1;
        auto scale2_3 = _outer.scale2_3;
        (void)scale2_3;
        auto scale4_5 = _outer.scale4_5;
        (void)scale4_5;
        auto scale6_7 = _outer.scale6_7;
        (void)scale6_7;
        auto scale8_9 = _outer.scale8_9;
        (void)scale8_9;
        auto scale10_11 = _outer.scale10_11;
        (void)scale10_11;
        auto scale12_13 = _outer.scale12_13;
        (void)scale12_13;
        auto scale14_15 = _outer.scale14_15;
        (void)scale14_15;
        auto bias0_1 = _outer.bias0_1;
        (void)bias0_1;
        auto bias2_3 = _outer.bias2_3;
        (void)bias2_3;
        auto bias4_5 = _outer.bias4_5;
        (void)bias4_5;
        auto bias6_7 = _outer.bias6_7;
        (void)bias6_7;
        auto bias8_9 = _outer.bias8_9;
        (void)bias8_9;
        auto bias10_11 = _outer.bias10_11;
        (void)bias10_11;
        auto bias12_13 = _outer.bias12_13;
        (void)bias12_13;
        auto bias14_15 = _outer.bias14_15;
        (void)bias14_15;
        auto thresholds1_7 = _outer.thresholds1_7;
        (void)thresholds1_7;
        auto thresholds9_13 = _outer.thresholds9_13;
        (void)thresholds9_13;
        scale0_1Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                       kFloat4_GrSLType, "scale0_1");
        if (intervalCount > 1) {
            scale2_3Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                           kFloat4_GrSLType, "scale2_3");
        }
        if (intervalCount > 2) {
            scale4_5Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                           kFloat4_GrSLType, "scale4_5");
        }
        if (intervalCount > 3) {
            scale6_7Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                           kFloat4_GrSLType, "scale6_7");
        }
        if (intervalCount > 4) {
            scale8_9Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                           kFloat4_GrSLType, "scale8_9");
        }
        if (intervalCount > 5) {
            scale10_11Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                             kFloat4_GrSLType, "scale10_11");
        }
        if (intervalCount > 6) {
            scale12_13Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                             kFloat4_GrSLType, "scale12_13");
        }
        if (intervalCount > 7) {
            scale14_15Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                             kFloat4_GrSLType, "scale14_15");
        }
        bias0_1Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                      kFloat4_GrSLType, "bias0_1");
        if (intervalCount > 1) {
            bias2_3Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                          kFloat4_GrSLType, "bias2_3");
        }
        if (intervalCount > 2) {
            bias4_5Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                          kFloat4_GrSLType, "bias4_5");
        }
        if (intervalCount > 3) {
            bias6_7Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                          kFloat4_GrSLType, "bias6_7");
        }
        if (intervalCount > 4) {
            bias8_9Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                          kFloat4_GrSLType, "bias8_9");
        }
        if (intervalCount > 5) {
            bias10_11Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                            kFloat4_GrSLType, "bias10_11");
        }
        if (intervalCount > 6) {
            bias12_13Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                            kFloat4_GrSLType, "bias12_13");
        }
        if (intervalCount > 7) {
            bias14_15Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                            kFloat4_GrSLType, "bias14_15");
        }
        thresholds1_7Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                            kHalf4_GrSLType, "thresholds1_7");
        thresholds9_13Var = args.fUniformHandler->addUniform(&_outer, kFragment_GrShaderFlag,
                                                             kHalf4_GrSLType, "thresholds9_13");
        fragBuilder->codeAppendf(
                R"SkSL(half t = half(%s.x);
float4 scale, bias;
if (%d <= 4 || t < %s.w) {
    if (%d <= 2 || t < %s.y) {
        if (%d <= 1 || t < %s.x) {
            scale = %s;
            bias = %s;
        } else {
            scale = %s;
            bias = %s;
        }
    } else {
        if (%d <= 3 || t < %s.z) {
            scale = %s;
            bias = %s;
        } else {
            scale = %s;
            bias = %s;
        }
    }
} else {
    if (%d <= 6 || t < %s.y) {
        if (%d <= 5 || t < %s.x) {
            scale = %s;
            bias = %s;
        } else {
            scale = %s;
            bias = %s;
        }
    } else {
        if (%d <= 7 || t < %s.z) {
            scale = %s;
            bias = %s;
        } else {
            scale = %s;
            bias = %s;
        }
    }
}
%s = half4(float(t) * scale + bias);
)SkSL",
                args.fSampleCoord, _outer.intervalCount,
                args.fUniformHandler->getUniformCStr(thresholds1_7Var), _outer.intervalCount,
                args.fUniformHandler->getUniformCStr(thresholds1_7Var), _outer.intervalCount,
                args.fUniformHandler->getUniformCStr(thresholds1_7Var),
                args.fUniformHandler->getUniformCStr(scale0_1Var),
                args.fUniformHandler->getUniformCStr(bias0_1Var),
                scale2_3Var.isValid() ? args.fUniformHandler->getUniformCStr(scale2_3Var)
                                      : "float4(0)",
                bias2_3Var.isValid() ? args.fUniformHandler->getUniformCStr(bias2_3Var)
                                     : "float4(0)",
                _outer.intervalCount, args.fUniformHandler->getUniformCStr(thresholds1_7Var),
                scale4_5Var.isValid() ? args.fUniformHandler->getUniformCStr(scale4_5Var)
                                      : "float4(0)",
                bias4_5Var.isValid() ? args.fUniformHandler->getUniformCStr(bias4_5Var)
                                     : "float4(0)",
                scale6_7Var.isValid() ? args.fUniformHandler->getUniformCStr(scale6_7Var)
                                      : "float4(0)",
                bias6_7Var.isValid() ? args.fUniformHandler->getUniformCStr(bias6_7Var)
                                     : "float4(0)",
                _outer.intervalCount, args.fUniformHandler->getUniformCStr(thresholds9_13Var),
                _outer.intervalCount, args.fUniformHandler->getUniformCStr(thresholds9_13Var),
                scale8_9Var.isValid() ? args.fUniformHandler->getUniformCStr(scale8_9Var)
                                      : "float4(0)",
                bias8_9Var.isValid() ? args.fUniformHandler->getUniformCStr(bias8_9Var)
                                     : "float4(0)",
                scale10_11Var.isValid() ? args.fUniformHandler->getUniformCStr(scale10_11Var)
                                        : "float4(0)",
                bias10_11Var.isValid() ? args.fUniformHandler->getUniformCStr(bias10_11Var)
                                       : "float4(0)",
                _outer.intervalCount, args.fUniformHandler->getUniformCStr(thresholds9_13Var),
                scale12_13Var.isValid() ? args.fUniformHandler->getUniformCStr(scale12_13Var)
                                        : "float4(0)",
                bias12_13Var.isValid() ? args.fUniformHandler->getUniformCStr(bias12_13Var)
                                       : "float4(0)",
                scale14_15Var.isValid() ? args.fUniformHandler->getUniformCStr(scale14_15Var)
                                        : "float4(0)",
                bias14_15Var.isValid() ? args.fUniformHandler->getUniformCStr(bias14_15Var)
                                       : "float4(0)",
                args.fOutputColor);
    }

private:
    void onSetData(const GrGLSLProgramDataManager& pdman,
                   const GrFragmentProcessor& _proc) override {
        const GrUnrolledBinaryGradientColorizer& _outer =
                _proc.cast<GrUnrolledBinaryGradientColorizer>();
        {
            pdman.set4fv(scale0_1Var, 1, (_outer.scale0_1).vec());
            if (scale2_3Var.isValid()) {
                pdman.set4fv(scale2_3Var, 1, (_outer.scale2_3).vec());
            }
            if (scale4_5Var.isValid()) {
                pdman.set4fv(scale4_5Var, 1, (_outer.scale4_5).vec());
            }
            if (scale6_7Var.isValid()) {
                pdman.set4fv(scale6_7Var, 1, (_outer.scale6_7).vec());
            }
            if (scale8_9Var.isValid()) {
                pdman.set4fv(scale8_9Var, 1, (_outer.scale8_9).vec());
            }
            if (scale10_11Var.isValid()) {
                pdman.set4fv(scale10_11Var, 1, (_outer.scale10_11).vec());
            }
            if (scale12_13Var.isValid()) {
                pdman.set4fv(scale12_13Var, 1, (_outer.scale12_13).vec());
            }
            if (scale14_15Var.isValid()) {
                pdman.set4fv(scale14_15Var, 1, (_outer.scale14_15).vec());
            }
            pdman.set4fv(bias0_1Var, 1, (_outer.bias0_1).vec());
            if (bias2_3Var.isValid()) {
                pdman.set4fv(bias2_3Var, 1, (_outer.bias2_3).vec());
            }
            if (bias4_5Var.isValid()) {
                pdman.set4fv(bias4_5Var, 1, (_outer.bias4_5).vec());
            }
            if (bias6_7Var.isValid()) {
                pdman.set4fv(bias6_7Var, 1, (_outer.bias6_7).vec());
            }
            if (bias8_9Var.isValid()) {
                pdman.set4fv(bias8_9Var, 1, (_outer.bias8_9).vec());
            }
            if (bias10_11Var.isValid()) {
                pdman.set4fv(bias10_11Var, 1, (_outer.bias10_11).vec());
            }
            if (bias12_13Var.isValid()) {
                pdman.set4fv(bias12_13Var, 1, (_outer.bias12_13).vec());
            }
            if (bias14_15Var.isValid()) {
                pdman.set4fv(bias14_15Var, 1, (_outer.bias14_15).vec());
            }
            pdman.set4fv(thresholds1_7Var, 1,
                         reinterpret_cast<const float*>(&(_outer.thresholds1_7)));
            pdman.set4fv(thresholds9_13Var, 1,
                         reinterpret_cast<const float*>(&(_outer.thresholds9_13)));
        }
    }
    UniformHandle scale0_1Var;
    UniformHandle scale2_3Var;
    UniformHandle scale4_5Var;
    UniformHandle scale6_7Var;
    UniformHandle scale8_9Var;
    UniformHandle scale10_11Var;
    UniformHandle scale12_13Var;
    UniformHandle scale14_15Var;
    UniformHandle bias0_1Var;
    UniformHandle bias2_3Var;
    UniformHandle bias4_5Var;
    UniformHandle bias6_7Var;
    UniformHandle bias8_9Var;
    UniformHandle bias10_11Var;
    UniformHandle bias12_13Var;
    UniformHandle bias14_15Var;
    UniformHandle thresholds1_7Var;
    UniformHandle thresholds9_13Var;
};
GrGLSLFragmentProcessor* GrUnrolledBinaryGradientColorizer::onCreateGLSLInstance() const {
    return new GrGLSLUnrolledBinaryGradientColorizer();
}
void GrUnrolledBinaryGradientColorizer::onGetGLSLProcessorKey(const GrShaderCaps& caps,
                                                              GrProcessorKeyBuilder* b) const {
    b->add32((uint32_t)intervalCount);
}
bool GrUnrolledBinaryGradientColorizer::onIsEqual(const GrFragmentProcessor& other) const {
    const GrUnrolledBinaryGradientColorizer& that = other.cast<GrUnrolledBinaryGradientColorizer>();
    (void)that;
    if (intervalCount != that.intervalCount) return false;
    if (scale0_1 != that.scale0_1) return false;
    if (scale2_3 != that.scale2_3) return false;
    if (scale4_5 != that.scale4_5) return false;
    if (scale6_7 != that.scale6_7) return false;
    if (scale8_9 != that.scale8_9) return false;
    if (scale10_11 != that.scale10_11) return false;
    if (scale12_13 != that.scale12_13) return false;
    if (scale14_15 != that.scale14_15) return false;
    if (bias0_1 != that.bias0_1) return false;
    if (bias2_3 != that.bias2_3) return false;
    if (bias4_5 != that.bias4_5) return false;
    if (bias6_7 != that.bias6_7) return false;
    if (bias8_9 != that.bias8_9) return false;
    if (bias10_11 != that.bias10_11) return false;
    if (bias12_13 != that.bias12_13) return false;
    if (bias14_15 != that.bias14_15) return false;
    if (thresholds1_7 != that.thresholds1_7) return false;
    if (thresholds9_13 != that.thresholds9_13) return false;
    return true;
}
bool GrUnrolledBinaryGradientColorizer::usesExplicitReturn() const { return false; }
GrUnrolledBinaryGradientColorizer::GrUnrolledBinaryGradientColorizer(
        const GrUnrolledBinaryGradientColorizer& src)
        : INHERITED(kGrUnrolledBinaryGradientColorizer_ClassID, src.optimizationFlags())
        , intervalCount(src.intervalCount)
        , scale0_1(src.scale0_1)
        , scale2_3(src.scale2_3)
        , scale4_5(src.scale4_5)
        , scale6_7(src.scale6_7)
        , scale8_9(src.scale8_9)
        , scale10_11(src.scale10_11)
        , scale12_13(src.scale12_13)
        , scale14_15(src.scale14_15)
        , bias0_1(src.bias0_1)
        , bias2_3(src.bias2_3)
        , bias4_5(src.bias4_5)
        , bias6_7(src.bias6_7)
        , bias8_9(src.bias8_9)
        , bias10_11(src.bias10_11)
        , bias12_13(src.bias12_13)
        , bias14_15(src.bias14_15)
        , thresholds1_7(src.thresholds1_7)
        , thresholds9_13(src.thresholds9_13) {
    this->cloneAndRegisterAllChildProcessors(src);
    this->setUsesSampleCoordsDirectly();
}
std::unique_ptr<GrFragmentProcessor> GrUnrolledBinaryGradientColorizer::clone() const {
    return std::make_unique<GrUnrolledBinaryGradientColorizer>(*this);
}
#if GR_TEST_UTILS
SkString GrUnrolledBinaryGradientColorizer::onDumpInfo() const {
    return SkStringPrintf(
            "(intervalCount=%d, scale0_1=float4(%f, %f, %f, %f), scale2_3=float4(%f, %f, %f, %f), "
            "scale4_5=float4(%f, %f, %f, %f), scale6_7=float4(%f, %f, %f, %f), scale8_9=float4(%f, "
            "%f, %f, %f), scale10_11=float4(%f, %f, %f, %f), scale12_13=float4(%f, %f, %f, %f), "
            "scale14_15=float4(%f, %f, %f, %f), bias0_1=float4(%f, %f, %f, %f), bias2_3=float4(%f, "
            "%f, %f, %f), bias4_5=float4(%f, %f, %f, %f), bias6_7=float4(%f, %f, %f, %f), "
            "bias8_9=float4(%f, %f, %f, %f), bias10_11=float4(%f, %f, %f, %f), "
            "bias12_13=float4(%f, %f, %f, %f), bias14_15=float4(%f, %f, %f, %f), "
            "thresholds1_7=half4(%f, %f, %f, %f), thresholds9_13=half4(%f, %f, %f, %f))",
            intervalCount, scale0_1.fR, scale0_1.fG, scale0_1.fB, scale0_1.fA, scale2_3.fR,
            scale2_3.fG, scale2_3.fB, scale2_3.fA, scale4_5.fR, scale4_5.fG, scale4_5.fB,
            scale4_5.fA, scale6_7.fR, scale6_7.fG, scale6_7.fB, scale6_7.fA, scale8_9.fR,
            scale8_9.fG, scale8_9.fB, scale8_9.fA, scale10_11.fR, scale10_11.fG, scale10_11.fB,
            scale10_11.fA, scale12_13.fR, scale12_13.fG, scale12_13.fB, scale12_13.fA,
            scale14_15.fR, scale14_15.fG, scale14_15.fB, scale14_15.fA, bias0_1.fR, bias0_1.fG,
            bias0_1.fB, bias0_1.fA, bias2_3.fR, bias2_3.fG, bias2_3.fB, bias2_3.fA, bias4_5.fR,
            bias4_5.fG, bias4_5.fB, bias4_5.fA, bias6_7.fR, bias6_7.fG, bias6_7.fB, bias6_7.fA,
            bias8_9.fR, bias8_9.fG, bias8_9.fB, bias8_9.fA, bias10_11.fR, bias10_11.fG,
            bias10_11.fB, bias10_11.fA, bias12_13.fR, bias12_13.fG, bias12_13.fB, bias12_13.fA,
            bias14_15.fR, bias14_15.fG, bias14_15.fB, bias14_15.fA, thresholds1_7.left(),
            thresholds1_7.top(), thresholds1_7.right(), thresholds1_7.bottom(),
            thresholds9_13.left(), thresholds9_13.top(), thresholds9_13.right(),
            thresholds9_13.bottom());
}
#endif

static const int kMaxIntervals = 8;
std::unique_ptr<GrFragmentProcessor> GrUnrolledBinaryGradientColorizer::Make(
        const SkPMColor4f* colors, const SkScalar* positions, int count) {
    // Depending on how the positions resolve into hard stops or regular stops, the number of
    // intervals specified by the number of colors/positions can change. For instance, a plain
    // 3 color gradient is two intervals, but a 4 color gradient with a hard stop is also
    // two intervals. At the most extreme end, an 8 interval gradient made entirely of hard
    // stops has 16 colors.

    if (count > kMaxColorCount) {
        // Definitely cannot represent this gradient configuration
        return nullptr;
    }

    // The raster implementation also uses scales and biases, but since they must be calculated
    // after the dst color space is applied, it limits our ability to cache their values.
    SkPMColor4f scales[kMaxIntervals];
    SkPMColor4f biases[kMaxIntervals];
    SkScalar thresholds[kMaxIntervals];

    int intervalCount = 0;

    for (int i = 0; i < count - 1; i++) {
        if (intervalCount >= kMaxIntervals) {
            // Already reached kMaxIntervals, and haven't run out of color stops so this
            // gradient cannot be represented by this shader.
            return nullptr;
        }

        SkScalar t0 = positions[i];
        SkScalar t1 = positions[i + 1];
        SkScalar dt = t1 - t0;
        // If the interval is empty, skip to the next interval. This will automatically create
        // distinct hard stop intervals as needed. It also protects against malformed gradients
        // that have repeated hard stops at the very beginning that are effectively unreachable.
        if (SkScalarNearlyZero(dt)) {
            continue;
        }

        auto c0 = Sk4f::Load(colors[i].vec());
        auto c1 = Sk4f::Load(colors[i + 1].vec());

        auto scale = (c1 - c0) / dt;
        auto bias = c0 - t0 * scale;

        scale.store(scales + intervalCount);
        bias.store(biases + intervalCount);
        thresholds[intervalCount] = t1;
        intervalCount++;
    }

    // For isEqual to make sense, set the unused values to something consistent
    for (int i = intervalCount; i < kMaxIntervals; i++) {
        scales[i] = SK_PMColor4fTRANSPARENT;
        biases[i] = SK_PMColor4fTRANSPARENT;
        thresholds[i] = 0.0;
    }

    return std::unique_ptr<GrFragmentProcessor>(new GrUnrolledBinaryGradientColorizer(
            intervalCount, scales[0], scales[1], scales[2], scales[3], scales[4], scales[5],
            scales[6], scales[7], biases[0], biases[1], biases[2], biases[3], biases[4], biases[5],
            biases[6], biases[7],
            SkRect::MakeLTRB(thresholds[0], thresholds[1], thresholds[2], thresholds[3]),
            SkRect::MakeLTRB(thresholds[4], thresholds[5], thresholds[6], 0.0)));
}
