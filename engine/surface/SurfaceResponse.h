#pragma once

#include <string>

namespace surface {

struct SurfaceMovementResponse {
    float speedMultiplier = 1.0F;
    float accelerationMultiplier = 1.0F;
    float brakingMultiplier = 1.0F;
    float lateralControlMultiplier = 1.0F;
};

struct SurfacePhysicsResponse {
    float effectiveFriction = 0.6F;
    float effectiveRestitution = 0.1F;
    float effectiveRoughness = 0.5F;
    float sinkDepth = 0.0F;
};

struct SurfaceAudioResponse {
    std::string event;
    float gain = 1.0F;
    float pitch = 1.0F;
};

struct SurfaceVfxResponse {
    std::string preset;
    float intensity = 0.0F;
    float decalOpacity = 0.0F;
};

struct SurfaceResponse {
    SurfaceMovementResponse movement;
    SurfacePhysicsResponse physics;
    SurfaceAudioResponse audio;
    SurfaceVfxResponse vfx;
};

}  // namespace surface
