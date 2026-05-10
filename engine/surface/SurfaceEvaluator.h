#pragma once

#include "SurfaceContact.h"
#include "SurfaceProfile.h"
#include "SurfaceResponse.h"
#include "SurfaceState.h"

namespace surface {

class SurfaceEvaluator {
public:
    static SurfaceResponse evaluate(const SurfaceProfile& profile,
                                    const SurfaceState& state,
                                    const SurfaceContact& contact);
};

}  // namespace surface
