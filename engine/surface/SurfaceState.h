#pragma once

#include <algorithm>

namespace surface {

struct SurfaceState {
    float wetness = 0.0F;          // 0..1
    float saturation = 0.0F;       // 0..1
    float contamination = 0.0F;    // 0..1
    float temperatureCelsius = 20.0F;

    float deformation = 0.0F;      // 0..1 runtime compression
    float compaction = 0.0F;       // 0..1 runtime densification
    float coverage = 1.0F;         // 0..1 contact area coverage

    float wear = 0.0F;             // 0..1
    float damage = 0.0F;           // 0..1

    void clamp01() {
        wetness = std::clamp(wetness, 0.0F, 1.0F);
        saturation = std::clamp(saturation, 0.0F, 1.0F);
        contamination = std::clamp(contamination, 0.0F, 1.0F);
        deformation = std::clamp(deformation, 0.0F, 1.0F);
        compaction = std::clamp(compaction, 0.0F, 1.0F);
        coverage = std::clamp(coverage, 0.0F, 1.0F);
        wear = std::clamp(wear, 0.0F, 1.0F);
        damage = std::clamp(damage, 0.0F, 1.0F);
    }
};

}  // namespace surface
