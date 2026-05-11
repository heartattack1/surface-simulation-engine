#include "SurfaceEvaluator.h"

#include <algorithm>
#include <cmath>

namespace surface {
namespace {

void computeDerivedValues(SurfaceResponse& r) {
    const float frictionSlip = 1.0F - clamp01(r.dynamicFriction);
    const float gripSlip = 1.0F - clamp01(r.grip);
    r.slipperiness = clamp01(std::max(frictionSlip, gripSlip));
}

void clampResponse(SurfaceResponse& r) {
    r.staticFriction = std::max(0.0F, r.staticFriction);
    r.dynamicFriction = std::max(0.0F, r.dynamicFriction);
    r.restitution = clamp01(r.restitution);
    r.hardness = clamp01(r.hardness);
    r.roughness = clamp01(r.roughness);
    r.grip = clamp01(r.grip);
    r.slipperiness = clamp01(r.slipperiness);
}

}  // namespace

SurfaceResponse SurfaceEvaluator::evaluate(const SurfaceProfile& profile,
                                           const SurfaceState& state,
                                           const SurfaceContact& contact) const {
    SurfaceState s = state;
    s.clamp01();

    SurfaceResponse r{};
    r.staticFriction = profile.physical.baseStaticFriction;
    r.dynamicFriction = profile.physical.baseDynamicFriction;
    r.restitution = profile.physical.baseRestitution;
    r.hardness = profile.physical.baseHardness;
    r.roughness = profile.physical.baseRoughness;
    r.grip = profile.physical.baseGrip;

    const float wetF = profile.curves.wetnessToFriction.evaluate(s.wetness) * s.wetness * profile.environmental.wetSlipFactor;
    r.staticFriction *= (1.0F - wetF);
    r.dynamicFriction *= (1.0F - wetF);
    r.grip *= (1.0F - wetF);
    r.wetnessAmount = s.wetness;

    const float satSoft = profile.curves.saturationToHardness.evaluate(s.saturation) * s.saturation * profile.environmental.saturationSofteningFactor;
    r.hardness *= (1.0F - satSoft);

    const float dust = s.dustLayer;
    const float dpen = dust * profile.environmental.dustGripPenalty;
    r.grip *= (1.0F - dpen);
    r.staticFriction *= (1.0F - dpen * 0.5F);
    r.dynamicFriction *= (1.0F - dpen);
    r.dustAmount = dust;

    const float water = s.waterLayer;
    r.dynamicFriction *= (1.0F - water * profile.environmental.wetSlipFactor);
    r.grip *= (1.0F - water * profile.environmental.wetSlipFactor);
    r.wetnessAmount = std::max(r.wetnessAmount, water);

    const float oil = s.oilLayer;
    const float oslip = oil * profile.curves.oilToFriction.evaluate(oil) * profile.environmental.oilSlipFactor;
    r.staticFriction *= (1.0F - oslip);
    r.dynamicFriction *= (1.0F - oslip * 1.2F);
    r.grip *= (1.0F - oslip);
    r.oilAmount = oil;

    const float ice = s.iceLayer;
    const float islip = ice * profile.curves.iceToFriction.evaluate(ice) * profile.environmental.iceSlipFactor;
    r.staticFriction *= (1.0F - islip);
    r.dynamicFriction *= (1.0F - islip);
    r.grip *= (1.0F - islip);
    r.iceAmount = ice;

    r.accelerationMultiplier *= profile.movement.accelerationMultiplier;
    r.brakingMultiplier *= profile.movement.brakingMultiplier;
    r.turnControlMultiplier *= profile.movement.turnControlMultiplier;
    r.jumpTakeoffMultiplier *= profile.movement.jumpTakeoffMultiplier;
    r.landingStabilityMultiplier *= profile.movement.landingStabilityMultiplier;

    if (profile.deformation.deformable) {
        r.canDeform = true;
        const float pressure = contact.contactPressure * profile.deformation.pressureToDeformationScale;
        const float denom = std::max(0.001F, r.hardness) * std::max(0.001F, profile.deformation.deformationResistance);
        r.deformationAmount = clamp01(profile.curves.pressureToDeformation.evaluate(clamp01(pressure)) * pressure / denom);
        r.deformationDepth = r.deformationAmount * profile.deformation.maxDeformationDepth;
        r.deformationPersistence = profile.deformation.deformationPersistence;
    }

    if (contact.interaction == SurfaceInteractionType::Footstep) {
        r.footstepIntensity = clamp01(contact.contactPressure) * (0.5F + r.roughness * 0.5F);
        r.audioWetness = clamp01((s.wetness + s.waterLayer) * profile.feedback.wetnessToAudio);
    } else if (contact.interaction == SurfaceInteractionType::Landing || contact.interaction == SurfaceInteractionType::Impact) {
        const float impact = std::abs(contact.normalSpeed) * contact.actorMass;
        r.impactIntensity = clamp01(impact / 20.0F);
        r.particleIntensity += r.impactIntensity * profile.feedback.impactToParticles;
        r.hapticIntensity += r.impactIntensity * profile.feedback.impactToHaptics;
    } else if (contact.interaction == SurfaceInteractionType::Sliding || contact.interaction == SurfaceInteractionType::Dragging) {
        const float slide = clamp01(contact.tangentialSpeed / 10.0F);
        r.slideIntensity = slide;
        r.particleIntensity += slide * profile.feedback.slideToParticles;
        r.cameraJitterHint += slide * r.roughness * profile.feedback.roughnessToCameraJitter;
    }

    computeDerivedValues(r);
    r.accelerationMultiplier *= (1.0F - 0.3F * r.slipperiness);
    r.brakingMultiplier *= (1.0F - 0.5F * r.slipperiness);

    clampResponse(r);
    return r;
}

}  // namespace surface
