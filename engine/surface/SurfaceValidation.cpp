#include "SurfaceValidation.h"

namespace surface {

namespace {

void addError(std::vector<SurfaceValidationError>& errors,
              SurfaceId id,
              const std::string& field,
              const std::string& message) {
    errors.push_back({id, field, message, SurfaceValidationSeverity::Error});
}

void validateUnitRange(const SurfaceProfile& profile,
                       std::vector<SurfaceValidationError>& errors,
                       const std::string& field,
                       float value) {
    if (value < 0.0F || value > 1.0F) {
        addError(errors, profile.id, field, "value must be within [0, 1]");
    }
}

}  // namespace

std::vector<SurfaceValidationError> validateSurfaceProfile(const SurfaceProfile& profile) {
    std::vector<SurfaceValidationError> errors;

    if (profile.id == kInvalidSurfaceId) {
        addError(errors, profile.id, "id", "surface id must be a non-zero value");
    }

    validateUnitRange(profile, errors, "physical.friction", profile.physical.friction);
    validateUnitRange(profile, errors, "physical.staticFriction", profile.physical.staticFriction);
    validateUnitRange(profile, errors, "physical.restitution", profile.physical.restitution);
    validateUnitRange(profile, errors, "physical.roughness", profile.physical.roughness);

    if (profile.physical.staticFriction < profile.physical.friction) {
        addError(errors,
                 profile.id,
                 "physical.staticFriction",
                 "static friction must be >= dynamic friction");
    }

    validateUnitRange(profile,
                      errors,
                      "environmental.thermalConductivity",
                      profile.environmental.thermalConductivity);
    validateUnitRange(profile, errors, "environmental.heatCapacity", profile.environmental.heatCapacity);
    validateUnitRange(profile, errors, "environmental.absorbency", profile.environmental.absorbency);

    validateUnitRange(profile, errors, "deformation.softness", profile.deformation.softness);
    validateUnitRange(profile, errors, "deformation.maxCompression", profile.deformation.maxCompression);
    validateUnitRange(profile, errors, "deformation.recoveryRate", profile.deformation.recoveryRate);

    if (profile.movement.speedMultiplier <= 0.0F) {
        addError(errors, profile.id, "movement.speedMultiplier", "value must be > 0");
    }
    if (profile.movement.accelerationMultiplier <= 0.0F) {
        addError(errors, profile.id, "movement.accelerationMultiplier", "value must be > 0");
    }
    if (profile.movement.brakingMultiplier <= 0.0F) {
        addError(errors, profile.id, "movement.brakingMultiplier", "value must be > 0");
    }
    if (profile.movement.lateralControlMultiplier <= 0.0F) {
        addError(errors, profile.id, "movement.lateralControlMultiplier", "value must be > 0");
    }

    return errors;
}

}  // namespace surface
