#pragma once

#include <cstdint>

namespace surface {

struct Vec3 {
    float x = 0.0F;
    float y = 0.0F;
    float z = 0.0F;
};

enum class SurfaceInteractionType : std::uint8_t {
    Unknown,
    Footstep,
    Landing,
    Sliding,
    Rolling,
    Dragging,
    Impact,
    StaticContact,
    ProjectileHit,
    LiquidContact,
};

struct SurfaceContact {
    SurfaceInteractionType interaction = SurfaceInteractionType::Unknown;
    Vec3 position{0.0F, 0.0F, 0.0F};
    Vec3 normal{0.0F, 1.0F, 0.0F};
    Vec3 relativeVelocity{0.0F, 0.0F, 0.0F};

    float normalSpeed = 0.0F;
    float tangentialSpeed = 0.0F;
    float contactPressure = 0.0F;
    float contactArea = 1.0F;
    float contactDuration = 0.0F;
    float actorMass = 1.0F;
    float impulse = 0.0F;
    std::uint32_t actorMaterialMask = 0;
};

}  // namespace surface
