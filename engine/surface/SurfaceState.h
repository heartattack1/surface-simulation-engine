#pragma once

#include <algorithm>

namespace surface {

struct SurfaceState {
    float wetness = 0.0F;
    float saturation = 0.0F;
    float temperature = 20.0F;
    float deformation = 0.0F;
    float damage = 0.0F;

    float waterLayer = 0.0F;
    float snowLayer = 0.0F;
    float iceLayer = 0.0F;
    float dustLayer = 0.0F;
    float oilLayer = 0.0F;

    void clamp01() {
        wetness = std::clamp(wetness, 0.0F, 1.0F);
        saturation = std::clamp(saturation, 0.0F, 1.0F);
        deformation = std::clamp(deformation, 0.0F, 1.0F);
        damage = std::clamp(damage, 0.0F, 1.0F);
        waterLayer = std::clamp(waterLayer, 0.0F, 1.0F);
        snowLayer = std::clamp(snowLayer, 0.0F, 1.0F);
        iceLayer = std::clamp(iceLayer, 0.0F, 1.0F);
        dustLayer = std::clamp(dustLayer, 0.0F, 1.0F);
        oilLayer = std::clamp(oilLayer, 0.0F, 1.0F);
    }
};

}  // namespace surface
