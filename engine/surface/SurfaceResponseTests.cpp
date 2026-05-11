#include <cassert>
#include <type_traits>
#include <unordered_map>

#include "SurfaceEvaluator.h"

using namespace surface;

int main() {
    static_assert(std::is_invocable_r_v<std::size_t, std::hash<SurfaceId>, const SurfaceId&>);

    std::unordered_map<SurfaceId, int, std::hash<SurfaceId>> surfaceMap;
    surfaceMap.emplace(SurfaceId{42}, 7);
    assert(surfaceMap.find(SurfaceId{42}) != surfaceMap.end());
    assert(surfaceMap.at(SurfaceId{42}) == 7);

    SurfaceProfile profile{};
    profile.id = SurfaceId{1};
    profile.physical.baseStaticFriction = 0.9F;
    profile.physical.baseDynamicFriction = 0.75F;
    profile.environmental.wetSlipFactor = 0.2F;

    SurfaceState state{};
    state.wetness = 1.0F;

    SurfaceContact contact{};
    contact.interaction = SurfaceInteractionType::Sliding;
    contact.tangentialSpeed = 7.0F;

    SurfaceEvaluator evaluator;
    const SurfaceResponse response = evaluator.evaluate(profile, state, contact);

    assert(response.dynamicFriction < 0.75F);
    assert(response.slideIntensity > 0.0F);
    assert(response.slipperiness >= 0.0F && response.slipperiness <= 1.0F);
    return 0;
}
