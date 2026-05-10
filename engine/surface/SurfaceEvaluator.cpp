#include "SurfaceEvaluator.h"

#include <algorithm>

namespace surface {

namespace {

float clamp01(float value) {
    return std::clamp(value, 0.0F, 1.0F);
}

}  // namespace

SurfaceResponse SurfaceEvaluator::evaluate(const SurfaceProfile& profile,
                                           const SurfaceState& state,
                                           const SurfaceContact& contact) {
    SurfaceState normalizedState = state;
    normalizedState.clamp01();

    const float wetPenalty = 0.35F * normalizedState.wetness;
    const float contaminationPenalty = 0.2F * normalizedState.contamination;
    const float wearPenalty = 0.15F * normalizedState.wear;

    const float tractionFactor = std::max(0.1F, 1.0F - wetPenalty - contaminationPenalty - wearPenalty);
    const float deformationFactor = clamp01(normalizedState.deformation + normalizedState.compaction * 0.5F);
    const float impactFactor = clamp01(contact.impactImpulse / 1000.0F);
    const float slipFactor = clamp01(contact.slipRatio);

    SurfaceResponse response;

    response.movement.speedMultiplier = profile.movement.speedMultiplier * (0.85F + 0.15F * tractionFactor);
    response.movement.accelerationMultiplier = profile.movement.accelerationMultiplier * tractionFactor;
    response.movement.brakingMultiplier = profile.movement.brakingMultiplier * (0.7F + 0.3F * tractionFactor);
    response.movement.lateralControlMultiplier =
        profile.movement.lateralControlMultiplier * (1.0F - 0.5F * slipFactor);

    response.physics.effectiveFriction = profile.physical.friction * tractionFactor;
    response.physics.effectiveRestitution = profile.physical.restitution * (1.0F - deformationFactor * 0.4F);
    response.physics.effectiveRoughness = profile.physical.roughness * (1.0F + normalizedState.coverage * 0.1F);
    response.physics.sinkDepth = profile.deformation.maxCompression * deformationFactor * (0.5F + 0.5F * impactFactor);

    response.audio.event = contact.isFootContact ? "surface.step" : "surface.contact";
    response.audio.gain = 0.4F + 0.6F * clamp01(contact.relativeSpeed / 15.0F);
    response.audio.pitch = 1.0F - 0.2F * deformationFactor + 0.1F * slipFactor;

    response.vfx.preset = (normalizedState.wetness > 0.6F) ? "surface.splash" : "surface.debris";
    response.vfx.intensity = clamp01(0.5F * impactFactor + 0.5F * slipFactor);
    response.vfx.decalOpacity = clamp01(normalizedState.coverage * (0.5F + 0.5F * normalizedState.saturation));

    return response;
}

}  // namespace surface
