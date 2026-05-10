#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace surface {

using SurfaceId = std::uint32_t;

constexpr SurfaceId kInvalidSurfaceId = 0;

enum class SurfaceCategory {
    Unknown = 0,
    Hard,
    Loose,
    Liquid,
    Organic,
    Synthetic,
};

enum class TractionClass {
    VeryLow = 0,
    Low,
    Medium,
    High,
    VeryHigh,
};

struct PhysicalProperties {
    float friction = 0.6F;
    float staticFriction = 0.7F;
    float restitution = 0.1F;
    float roughness = 0.5F;
    TractionClass tractionClass = TractionClass::Medium;
};

struct EnvironmentalProperties {
    float thermalConductivity = 0.5F;
    float heatCapacity = 0.5F;
    float absorbency = 0.5F;
};

struct DeformationProperties {
    float softness = 0.1F;
    float maxCompression = 0.05F;
    float recoveryRate = 0.9F;
};

struct MovementProperties {
    float speedMultiplier = 1.0F;
    float accelerationMultiplier = 1.0F;
    float brakingMultiplier = 1.0F;
    float lateralControlMultiplier = 1.0F;
};

using SurfaceTags = std::vector<std::string>;
using SurfaceMetadata = std::unordered_map<std::string, std::string>;

}  // namespace surface
