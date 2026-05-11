#pragma once

#include <string>

#include "SurfaceTypes.h"

namespace surface {

struct PhysicalSurfaceProperties {
    float baseStaticFriction = 1.0F;
    float baseDynamicFriction = 0.8F;
    float baseRestitution = 0.0F;
    float baseHardness = 1.0F;
    float baseRoughness = 0.0F;
    float baseGrip = 1.0F;
};

struct EnvironmentalResponseProperties {
    float wetSlipFactor = 0.0F;
    float saturationSofteningFactor = 0.0F;
    float iceSlipFactor = 0.0F;
    float oilSlipFactor = 0.0F;
    float dustGripPenalty = 0.0F;
};

struct DeformationProperties {
    bool deformable = false;
    float deformationResistance = 1.0F;
    float maxDeformationDepth = 0.0F;
    float deformationPersistence = 0.0F;
    float deformationRecoveryRate = 0.0F;
    float pressureToDeformationScale = 1.0F;
};

struct MovementResponseProperties {
    float accelerationMultiplier = 1.0F;
    float brakingMultiplier = 1.0F;
    float turnControlMultiplier = 1.0F;
    float jumpTakeoffMultiplier = 1.0F;
    float landingStabilityMultiplier = 1.0F;
};

struct FeedbackResponseProperties {
    float roughnessToCameraJitter = 0.0F;
    float wetnessToAudio = 1.0F;
    float impactToParticles = 1.0F;
    float slideToParticles = 1.0F;
    float impactToHaptics = 1.0F;
};

struct SurfaceResponseCurves {
    Curve wetnessToFriction{};
    Curve saturationToHardness{};
    Curve iceToFriction{};
    Curve oilToFriction{};
    Curve pressureToDeformation{};
    Curve impactToParticleIntensity{};
    Curve slideSpeedToFeedback{};
};

struct SurfaceProfile {
    SurfaceId id = kInvalidSurfaceId;
    std::string name;
    SurfaceCategory category = SurfaceCategory::Unknown;

    PhysicalSurfaceProperties physical;
    EnvironmentalResponseProperties environmental;
    DeformationProperties deformation;
    MovementResponseProperties movement;
    FeedbackResponseProperties feedback;
    SurfaceResponseCurves curves;
};

}  // namespace surface
