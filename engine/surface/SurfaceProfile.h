#pragma once

#include <string>

#include "SurfaceTypes.h"

namespace surface {

struct SurfaceProfile {
    SurfaceId id = kInvalidSurfaceId;
    std::string name;
    SurfaceCategory category = SurfaceCategory::Unknown;

    PhysicalProperties physical;
    EnvironmentalProperties environmental;
    DeformationProperties deformation;
    MovementProperties movement;

    SurfaceTags tags;
    SurfaceMetadata metadata;
};

}  // namespace surface
