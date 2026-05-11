#include "SurfaceValidation.h"

namespace surface {
namespace {

void addIssue(std::vector<SurfaceValidationError>& errors,
              SurfaceId id,
              const std::string& field,
              const std::string& message,
              SurfaceValidationSeverity severity = SurfaceValidationSeverity::Error) {
    errors.push_back({id, field, message, severity});
}

bool inRange(float v, float lo, float hi) { return v >= lo && v <= hi; }

void validateCurveSamples(std::vector<SurfaceValidationError>& errors,
                        SurfaceId id,
                        const Curve& curve,
                        const std::string& field) {
    for (std::size_t i = 0; i < curve.samples.size(); ++i) {
        if (!inRange(curve.samples[i], 0.0F, 1.0F)) {
            addIssue(errors, id, field + ".samples[" + std::to_string(i) + "]", "must be in [0,1]");
        }
    }
}

}  // namespace

std::vector<SurfaceValidationError> validateSurfaceProfile(const SurfaceProfile& p) {
    std::vector<SurfaceValidationError> errors;
    if (p.id == kInvalidSurfaceId) addIssue(errors, p.id, "id", "surface id must be non-zero");
    if (p.physical.baseStaticFriction < 0.0F) addIssue(errors, p.id, "physical.baseStaticFriction", "must be >= 0");
    if (p.physical.baseDynamicFriction < 0.0F) addIssue(errors, p.id, "physical.baseDynamicFriction", "must be >= 0");
    if (p.physical.baseStaticFriction < p.physical.baseDynamicFriction) addIssue(errors, p.id, "physical", "baseStaticFriction must be >= baseDynamicFriction");
    if (!inRange(p.physical.baseRestitution, 0.0F, 1.0F)) addIssue(errors, p.id, "physical.baseRestitution", "must be in [0,1]");
    if (!inRange(p.physical.baseHardness, 0.0F, 1.0F)) addIssue(errors, p.id, "physical.baseHardness", "must be in [0,1]");
    if (!inRange(p.physical.baseRoughness, 0.0F, 1.0F)) addIssue(errors, p.id, "physical.baseRoughness", "must be in [0,1]");
    if (!inRange(p.physical.baseGrip, 0.0F, 1.0F)) addIssue(errors, p.id, "physical.baseGrip", "must be in [0,1]");

    if (!inRange(p.environmental.wetSlipFactor, 0.0F, 1.0F)) addIssue(errors, p.id, "environmental.wetSlipFactor", "must be in [0,1]");
    if (!inRange(p.environmental.saturationSofteningFactor, 0.0F, 1.0F)) addIssue(errors, p.id, "environmental.saturationSofteningFactor", "must be in [0,1]");
    if (!inRange(p.environmental.iceSlipFactor, 0.0F, 1.0F)) addIssue(errors, p.id, "environmental.iceSlipFactor", "must be in [0,1]");
    if (!inRange(p.environmental.oilSlipFactor, 0.0F, 1.0F)) addIssue(errors, p.id, "environmental.oilSlipFactor", "must be in [0,1]");
    if (!inRange(p.environmental.dustGripPenalty, 0.0F, 1.0F)) addIssue(errors, p.id, "environmental.dustGripPenalty", "must be in [0,1]");

    if (!inRange(p.movement.accelerationMultiplier, 0.0F, 2.0F)) addIssue(errors, p.id, "movement.accelerationMultiplier", "must be in [0,2]");
    if (!inRange(p.movement.brakingMultiplier, 0.0F, 2.0F)) addIssue(errors, p.id, "movement.brakingMultiplier", "must be in [0,2]");
    if (!inRange(p.movement.turnControlMultiplier, 0.0F, 2.0F)) addIssue(errors, p.id, "movement.turnControlMultiplier", "must be in [0,2]");
    if (!inRange(p.movement.jumpTakeoffMultiplier, 0.0F, 2.0F)) addIssue(errors, p.id, "movement.jumpTakeoffMultiplier", "must be in [0,2]");
    if (!inRange(p.movement.landingStabilityMultiplier, 0.0F, 2.0F)) addIssue(errors, p.id, "movement.landingStabilityMultiplier", "must be in [0,2]");

    if (!inRange(p.feedback.roughnessToCameraJitter, 0.0F, 2.0F)) addIssue(errors, p.id, "feedback.roughnessToCameraJitter", "must be in [0,2]", SurfaceValidationSeverity::Warning);
    if (!inRange(p.feedback.wetnessToAudio, 0.0F, 2.0F)) addIssue(errors, p.id, "feedback.wetnessToAudio", "must be in [0,2]", SurfaceValidationSeverity::Warning);
    if (!inRange(p.feedback.impactToParticles, 0.0F, 2.0F)) addIssue(errors, p.id, "feedback.impactToParticles", "must be in [0,2]", SurfaceValidationSeverity::Warning);
    if (!inRange(p.feedback.slideToParticles, 0.0F, 2.0F)) addIssue(errors, p.id, "feedback.slideToParticles", "must be in [0,2]", SurfaceValidationSeverity::Warning);
    if (!inRange(p.feedback.impactToHaptics, 0.0F, 2.0F)) addIssue(errors, p.id, "feedback.impactToHaptics", "must be in [0,2]", SurfaceValidationSeverity::Warning);
    validateCurveSamples(errors, p.id, p.curves.wetnessToFriction, "curves.wetnessToFriction");
    validateCurveSamples(errors, p.id, p.curves.saturationToHardness, "curves.saturationToHardness");
    validateCurveSamples(errors, p.id, p.curves.iceToFriction, "curves.iceToFriction");
    validateCurveSamples(errors, p.id, p.curves.oilToFriction, "curves.oilToFriction");
    validateCurveSamples(errors, p.id, p.curves.pressureToDeformation, "curves.pressureToDeformation");
    validateCurveSamples(errors, p.id, p.curves.impactToParticleIntensity, "curves.impactToParticleIntensity");
    validateCurveSamples(errors, p.id, p.curves.slideSpeedToFeedback, "curves.slideSpeedToFeedback");

    if (p.deformation.deformable && p.deformation.maxDeformationDepth <= 0.0F) addIssue(errors, p.id, "deformation.maxDeformationDepth", "deformable surface must have maxDeformationDepth > 0");
    if (!inRange(p.deformation.deformationPersistence, 0.0F, 1.0F)) addIssue(errors, p.id, "deformation.deformationPersistence", "must be in [0,1]");
    if (!inRange(p.deformation.deformationRecoveryRate, 0.0F, 1.0F)) addIssue(errors, p.id, "deformation.deformationRecoveryRate", "must be in [0,1]", SurfaceValidationSeverity::Warning);
    if (p.deformation.deformationResistance <= 0.0F) addIssue(errors, p.id, "deformation.deformationResistance", "must be > 0");
    if (p.deformation.pressureToDeformationScale < 0.0F) addIssue(errors, p.id, "deformation.pressureToDeformationScale", "must be >= 0");
    if (!p.deformation.deformable && p.deformation.maxDeformationDepth > 0.0F) addIssue(errors, p.id, "deformation.maxDeformationDepth", "non-deformable surface should keep depth at 0", SurfaceValidationSeverity::Warning);
    return errors;
}

std::vector<SurfaceValidationError> validateSurfaceState(const SurfaceState& s) {
    std::vector<SurfaceValidationError> errors;
    if (!inRange(s.wetness, 0.0F, 1.0F)) addIssue(errors, kInvalidSurfaceId, "state.wetness", "must be in [0,1]");
    if (!inRange(s.saturation, 0.0F, 1.0F)) addIssue(errors, kInvalidSurfaceId, "state.saturation", "must be in [0,1]");
    if (!inRange(s.waterLayer, 0.0F, 1.0F)) addIssue(errors, kInvalidSurfaceId, "state.waterLayer", "must be in [0,1]");
    if (!inRange(s.snowLayer, 0.0F, 1.0F)) addIssue(errors, kInvalidSurfaceId, "state.snowLayer", "must be in [0,1]");
    if (!inRange(s.iceLayer, 0.0F, 1.0F)) addIssue(errors, kInvalidSurfaceId, "state.iceLayer", "must be in [0,1]");
    if (!inRange(s.dustLayer, 0.0F, 1.0F)) addIssue(errors, kInvalidSurfaceId, "state.dustLayer", "must be in [0,1]");
    if (!inRange(s.oilLayer, 0.0F, 1.0F)) addIssue(errors, kInvalidSurfaceId, "state.oilLayer", "must be in [0,1]");
    if (!inRange(s.deformation, 0.0F, 1.0F)) addIssue(errors, kInvalidSurfaceId, "state.deformation", "must be in [0,1]");
    if (!inRange(s.damage, 0.0F, 1.0F)) addIssue(errors, kInvalidSurfaceId, "state.damage", "must be in [0,1]");
    return errors;
}

}  // namespace surface
