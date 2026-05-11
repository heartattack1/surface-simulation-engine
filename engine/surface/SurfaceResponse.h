#pragma once

#include <cstdint>

namespace surface {

struct SurfaceResponse {
    float staticFriction = 1.0F;
    float dynamicFriction = 1.0F;
    float restitution = 0.0F;
    float hardness = 1.0F;
    float roughness = 0.0F;
    float grip = 1.0F;
    float slipperiness = 0.0F;

    bool canDeform = false;
    float deformationAmount = 0.0F;
    float deformationDepth = 0.0F;
    float deformationPersistence = 0.0F;

    float wetnessAmount = 0.0F;
    float snowAmount = 0.0F;
    float dustAmount = 0.0F;
    float oilAmount = 0.0F;
    float iceAmount = 0.0F;

    float accelerationMultiplier = 1.0F;
    float brakingMultiplier = 1.0F;
    float turnControlMultiplier = 1.0F;
    float jumpTakeoffMultiplier = 1.0F;
    float landingStabilityMultiplier = 1.0F;

    float impactIntensity = 0.0F;
    float slideIntensity = 0.0F;
    float footstepIntensity = 0.0F;
    float particleIntensity = 0.0F;
    float audioWetness = 0.0F;
    float cameraJitterHint = 0.0F;
    float hapticIntensity = 0.0F;

    std::uint32_t flags = 0;
};

}  // namespace surface
