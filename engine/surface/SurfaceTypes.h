#pragma once

#include <algorithm>
#include <array>
#include <cstdint>

namespace surface {

struct SurfaceId {
    std::uint32_t value = 0;

    constexpr bool operator==(const SurfaceId&) const = default;
};

inline constexpr SurfaceId kInvalidSurfaceId{};

constexpr float clamp01(float value) {
    return std::clamp(value, 0.0F, 1.0F);
}

struct Curve {
    std::array<float, 4> samples{1.0F, 1.0F, 1.0F, 1.0F};

    float evaluate(float x) const {
        const float t = clamp01(x) * static_cast<float>(samples.size() - 1U);
        const std::size_t index = static_cast<std::size_t>(t);
        if (index >= samples.size() - 1U) {
            return samples.back();
        }
        const float frac = t - static_cast<float>(index);
        return samples[index] + (samples[index + 1U] - samples[index]) * frac;
    }
};

enum class SurfaceCategory : std::uint8_t {
    Unknown = 0,
    Hard,
    Loose,
    Liquid,
    Organic,
    Synthetic,
};

}  // namespace surface
