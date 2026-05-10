#pragma once

namespace surface {

struct SurfaceContact {
    float normalForce = 0.0F;
    float relativeSpeed = 0.0F;
    float slipRatio = 0.0F;      // 0..1
    float impactImpulse = 0.0F;
    float contactArea = 1.0F;
    bool isFootContact = false;
    bool isRollingContact = false;
};

}  // namespace surface
