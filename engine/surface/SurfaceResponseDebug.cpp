#include "SurfaceResponseDebug.h"

namespace surface {

SurfaceResponse evaluateSurfaceDebug(const SurfaceEvaluator& evaluator,
                                     const SurfaceProfile& profile,
                                     const SurfaceState& state,
                                     const SurfaceContact& contact,
                                     SurfaceResponseDebugInfo& debug) {
    const SurfaceResponse response = evaluator.evaluate(profile, state, contact);
    debug.entries.push_back({"dynamicFriction", profile.physical.baseDynamicFriction, response.dynamicFriction, "final evaluated value"});
    debug.entries.push_back({"hardness", profile.physical.baseHardness, response.hardness, "after saturation/deformation"});
    debug.entries.push_back({"slipperiness", 0.0F, response.slipperiness, "derived from friction+grip"});
    return response;
}

}  // namespace surface
