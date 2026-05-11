#pragma once

#include <string>
#include <vector>

#include "SurfaceContact.h"
#include "SurfaceEvaluator.h"

namespace surface {

struct SurfaceResponseDebugEntry {
    std::string name;
    float beforeValue = 0.0F;
    float afterValue = 0.0F;
    std::string reason;
};

struct SurfaceResponseDebugInfo {
    std::vector<SurfaceResponseDebugEntry> entries;
};

SurfaceResponse evaluateSurfaceDebug(const SurfaceEvaluator& evaluator,
                                     const SurfaceProfile& profile,
                                     const SurfaceState& state,
                                     const SurfaceContact& contact,
                                     SurfaceResponseDebugInfo& debug);

}  // namespace surface
