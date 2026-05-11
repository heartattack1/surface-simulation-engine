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

}  // namespace

std::vector<SurfaceValidationError> validateSurfaceProfile(const SurfaceProfile& p) {
    std::vector<SurfaceValidationError> errors;
    if (p.id == kInvalidSurfaceId) addIssue(errors, p.id, "id", "surface id must be non-zero");
    if (p.physical.baseStaticFriction < 0.0F) addIssue(errors, p.id, "physical.baseStaticFriction", "must be >= 0");
    if (p.physical.baseDynamicFriction < 0.0F) addIssue(errors, p.id, "physical.baseDynamicFriction", "must be >= 0");
    if (p.physical.baseStaticFriction < p.physical.baseDynamicFriction) addIssue(errors, p.id, "physical", "baseStaticFriction must be >= baseDynamicFriction");
    if (!inRange(p.physical.baseHardness, 0.0F, 1.0F)) addIssue(errors, p.id, "physical.baseHardness", "must be in [0,1]");
    if (!inRange(p.movement.accelerationMultiplier, 0.0F, 2.0F)) addIssue(errors, p.id, "movement.accelerationMultiplier", "must be in [0,2]");
    if (!inRange(p.movement.brakingMultiplier, 0.0F, 2.0F)) addIssue(errors, p.id, "movement.brakingMultiplier", "must be in [0,2]");
    if (p.deformation.deformable && p.deformation.maxDeformationDepth <= 0.0F) addIssue(errors, p.id, "deformation.maxDeformationDepth", "deformable surface must have maxDeformationDepth > 0");
    if (!p.deformation.deformable && p.deformation.maxDeformationDepth > 0.0F) addIssue(errors, p.id, "deformation.maxDeformationDepth", "non-deformable surface should keep depth at 0", SurfaceValidationSeverity::Warning);
    return errors;
}

std::vector<SurfaceValidationError> validateSurfaceState(const SurfaceState& s) {
    std::vector<SurfaceValidationError> errors;
    if (!inRange(s.wetness, 0.0F, 1.0F)) addIssue(errors, kInvalidSurfaceId, "state.wetness", "must be in [0,1]");
    if (!inRange(s.saturation, 0.0F, 1.0F)) addIssue(errors, kInvalidSurfaceId, "state.saturation", "must be in [0,1]");
    if (!inRange(s.waterLayer, 0.0F, 1.0F)) addIssue(errors, kInvalidSurfaceId, "state.waterLayer", "must be in [0,1]");
    return errors;
}

}  // namespace surface
