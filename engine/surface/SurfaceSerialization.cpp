#include "SurfaceSerialization.h"

#include <charconv>
#include <fstream>
#include <sstream>
#include <string_view>

namespace surface {

namespace {

bool extractObject(std::string_view src, std::string_view key, std::string_view& out) {
    const std::string marker = "\"" + std::string(key) + "\"";
    const std::size_t keyPos = src.find(marker);
    if (keyPos == std::string_view::npos) {
        return false;
    }

    const std::size_t colonPos = src.find(':', keyPos + marker.size());
    if (colonPos == std::string_view::npos) {
        return false;
    }

    const std::size_t bracePos = src.find('{', colonPos + 1U);
    if (bracePos == std::string_view::npos) {
        return false;
    }

    int depth = 0;
    for (std::size_t i = bracePos; i < src.size(); ++i) {
        if (src[i] == '{') {
            ++depth;
        } else if (src[i] == '}') {
            --depth;
            if (depth == 0) {
                out = src.substr(bracePos, i - bracePos + 1U);
                return true;
            }
        }
    }

    return false;
}

bool extractString(std::string_view src, std::string_view key, std::string& out) {
    const std::string marker = "\"" + std::string(key) + "\"";
    const std::size_t keyPos = src.find(marker);
    if (keyPos == std::string_view::npos) {
        return false;
    }

    const std::size_t colonPos = src.find(':', keyPos + marker.size());
    if (colonPos == std::string_view::npos) {
        return false;
    }

    const std::size_t firstQuote = src.find('"', colonPos + 1U);
    if (firstQuote == std::string_view::npos) {
        return false;
    }

    const std::size_t secondQuote = src.find('"', firstQuote + 1U);
    if (secondQuote == std::string_view::npos) {
        return false;
    }

    out = std::string(src.substr(firstQuote + 1U, secondQuote - firstQuote - 1U));
    return true;
}

bool extractUnsigned(std::string_view src, std::string_view key, SurfaceId& out) {
    const std::string marker = "\"" + std::string(key) + "\"";
    const std::size_t keyPos = src.find(marker);
    if (keyPos == std::string_view::npos) {
        return false;
    }

    const std::size_t colonPos = src.find(':', keyPos + marker.size());
    if (colonPos == std::string_view::npos) {
        return false;
    }

    const char* begin = src.data() + colonPos + 1U;
    const char* end = src.data() + src.size();
    while (begin < end && (*begin == ' ' || *begin == '\n' || *begin == '\t')) {
        ++begin;
    }

    std::uint32_t parsed = 0;
    const auto [ptr, ec] = std::from_chars(begin, end, parsed);
    if (ec != std::errc()) {
        return false;
    }

    out = static_cast<SurfaceId>(parsed);
    return ptr > begin;
}

bool extractFloat(std::string_view src, std::string_view key, float& out) {
    const std::string marker = "\"" + std::string(key) + "\"";
    const std::size_t keyPos = src.find(marker);
    if (keyPos == std::string_view::npos) {
        return false;
    }

    const std::size_t colonPos = src.find(':', keyPos + marker.size());
    if (colonPos == std::string_view::npos) {
        return false;
    }

    const std::size_t valueStart = src.find_first_of("-0123456789", colonPos + 1U);
    if (valueStart == std::string_view::npos) {
        return false;
    }

    const std::size_t valueEnd = src.find_first_not_of("-+.0123456789eE", valueStart);
    const std::string token = std::string(src.substr(valueStart, valueEnd - valueStart));

    try {
        out = std::stof(token);
        return true;
    } catch (...) {
        return false;
    }
}

bool extractBool(std::string_view src, std::string_view key, bool& out) {
    const std::string marker = "\"" + std::string(key) + "\"";
    const std::size_t keyPos = src.find(marker);
    if (keyPos == std::string_view::npos) {
        return false;
    }

    const std::size_t colonPos = src.find(':', keyPos + marker.size());
    if (colonPos == std::string_view::npos) {
        return false;
    }

    const std::size_t valueStart = src.find_first_not_of(" \n\t", colonPos + 1U);
    if (valueStart == std::string_view::npos) {
        return false;
    }

    if (src.substr(valueStart, 4U) == "true") {
        out = true;
        return true;
    }
    if (src.substr(valueStart, 5U) == "false") {
        out = false;
        return true;
    }
    return false;
}



void setFlatCurve(Curve& curve, float value) {
    curve.samples.fill(value);
}

SurfaceCategory categoryFromString(const std::string& category) {
    if (category == "Hard") return SurfaceCategory::Hard;
    if (category == "Loose") return SurfaceCategory::Loose;
    if (category == "Liquid") return SurfaceCategory::Liquid;
    if (category == "Organic") return SurfaceCategory::Organic;
    if (category == "Synthetic") return SurfaceCategory::Synthetic;
    return SurfaceCategory::Unknown;
}

SurfaceParseResult parseSurfaceProfileV0(std::string_view root) {
    SurfaceParseResult result;
    if (!extractUnsigned(root, "id", result.profile.id)) {
        result.error = "v0: missing/invalid key 'id'";
        return result;
    }
    if (!extractString(root, "name", result.profile.name)) {
        result.error = "v0: missing/invalid key 'name'";
        return result;
    }

    std::string categoryValue;
    if (!extractString(root, "category", categoryValue)) {
        result.error = "v0: missing/invalid key 'category'";
        return result;
    }
    result.profile.category = categoryFromString(categoryValue);

    std::string_view physical;
    std::string_view environmental;
    std::string_view deformation;
    std::string_view movement;
    if (!extractObject(root, "physical", physical)) {
        result.error = "v0: missing object 'physical'";
        return result;
    }
    if (!extractObject(root, "environmental", environmental)) {
        result.error = "v0: missing object 'environmental'";
        return result;
    }
    if (!extractObject(root, "deformation", deformation)) {
        result.error = "v0: missing object 'deformation'";
        return result;
    }
    if (!extractObject(root, "movement", movement)) {
        result.error = "v0: missing object 'movement'";
        return result;
    }

    if (!extractFloat(physical, "friction", result.profile.physical.baseDynamicFriction)) {
        result.error = "v0: missing/invalid key 'physical.friction'";
        return result;
    }
    if (!extractFloat(physical, "staticFriction", result.profile.physical.baseStaticFriction)) {
        result.error = "v0: missing/invalid key 'physical.staticFriction'";
        return result;
    }
    if (!extractFloat(physical, "restitution", result.profile.physical.baseRestitution)) {
        result.error = "v0: missing/invalid key 'physical.restitution'";
        return result;
    }
    if (!extractFloat(physical, "roughness", result.profile.physical.baseRoughness)) {
        result.error = "v0: missing/invalid key 'physical.roughness'";
        return result;
    }

    if (!extractFloat(environmental, "absorbency", result.profile.environmental.saturationSofteningFactor)) {
        result.error = "v0: missing/invalid key 'environmental.absorbency'";
        return result;
    }

    if (!extractFloat(deformation, "softness", result.profile.deformation.deformationPersistence)) {
        result.error = "v0: missing/invalid key 'deformation.softness'";
        return result;
    }
    if (!extractFloat(deformation, "maxCompression", result.profile.deformation.maxDeformationDepth)) {
        result.error = "v0: missing/invalid key 'deformation.maxCompression'";
        return result;
    }
    if (!extractFloat(deformation, "recoveryRate", result.profile.deformation.deformationRecoveryRate)) {
        result.error = "v0: missing/invalid key 'deformation.recoveryRate'";
        return result;
    }
    result.profile.deformation.deformable = result.profile.deformation.maxDeformationDepth > 0.0F;

    if (!extractFloat(movement, "accelerationMultiplier", result.profile.movement.accelerationMultiplier)) {
        result.error = "v0: missing/invalid key 'movement.accelerationMultiplier'";
        return result;
    }
    if (!extractFloat(movement, "brakingMultiplier", result.profile.movement.brakingMultiplier)) {
        result.error = "v0: missing/invalid key 'movement.brakingMultiplier'";
        return result;
    }
    if (!extractFloat(movement, "lateralControlMultiplier", result.profile.movement.turnControlMultiplier)) {
        result.error = "v0: missing/invalid key 'movement.lateralControlMultiplier'";
        return result;
    }

    result.ok = true;
    return result;
}

SurfaceParseResult parseSurfaceProfileV1(std::string_view root) {
    SurfaceParseResult result;
    if (!extractUnsigned(root, "id", result.profile.id)) {
        result.error = "v1: missing/invalid key 'id'";
        return result;
    }
    if (!extractString(root, "name", result.profile.name)) {
        result.error = "v1: missing/invalid key 'name'";
        return result;
    }
    std::string categoryValue;
    if (!extractString(root, "category", categoryValue)) {
        result.error = "v1: missing/invalid key 'category'";
        return result;
    }
    result.profile.category = categoryFromString(categoryValue);

    std::string_view physical;
    std::string_view environmental;
    std::string_view deformation;
    std::string_view movement;
    std::string_view feedback;
    std::string_view curves;

    if (!extractObject(root, "physical", physical)) { result.error = "v1: missing object 'physical'"; return result; }
    if (!extractObject(root, "environmental", environmental)) { result.error = "v1: missing object 'environmental'"; return result; }
    if (!extractObject(root, "deformation", deformation)) { result.error = "v1: missing object 'deformation'"; return result; }
    if (!extractObject(root, "movement", movement)) { result.error = "v1: missing object 'movement'"; return result; }
    if (!extractObject(root, "feedback", feedback)) { result.error = "v1: missing object 'feedback'"; return result; }
    if (!extractObject(root, "curves", curves)) { result.error = "v1: missing object 'curves'"; return result; }

#define REQF(obj, key, target, path) if (!extractFloat((obj), (key), (target))) { result.error = "v1: missing/invalid key '" path "'"; return result; }

    REQF(physical, "baseStaticFriction", result.profile.physical.baseStaticFriction, "physical.baseStaticFriction");
    REQF(physical, "baseDynamicFriction", result.profile.physical.baseDynamicFriction, "physical.baseDynamicFriction");
    REQF(physical, "baseRestitution", result.profile.physical.baseRestitution, "physical.baseRestitution");
    REQF(physical, "baseHardness", result.profile.physical.baseHardness, "physical.baseHardness");
    REQF(physical, "baseRoughness", result.profile.physical.baseRoughness, "physical.baseRoughness");
    REQF(physical, "baseGrip", result.profile.physical.baseGrip, "physical.baseGrip");

    REQF(environmental, "wetSlipFactor", result.profile.environmental.wetSlipFactor, "environmental.wetSlipFactor");
    REQF(environmental, "saturationSofteningFactor", result.profile.environmental.saturationSofteningFactor, "environmental.saturationSofteningFactor");
    REQF(environmental, "iceSlipFactor", result.profile.environmental.iceSlipFactor, "environmental.iceSlipFactor");
    REQF(environmental, "oilSlipFactor", result.profile.environmental.oilSlipFactor, "environmental.oilSlipFactor");
    REQF(environmental, "dustGripPenalty", result.profile.environmental.dustGripPenalty, "environmental.dustGripPenalty");

    if (!extractBool(deformation, "deformable", result.profile.deformation.deformable)) { result.error = "v1: missing/invalid key 'deformation.deformable'"; return result; }
    REQF(deformation, "deformationResistance", result.profile.deformation.deformationResistance, "deformation.deformationResistance");
    REQF(deformation, "maxDeformationDepth", result.profile.deformation.maxDeformationDepth, "deformation.maxDeformationDepth");
    REQF(deformation, "deformationPersistence", result.profile.deformation.deformationPersistence, "deformation.deformationPersistence");
    REQF(deformation, "deformationRecoveryRate", result.profile.deformation.deformationRecoveryRate, "deformation.deformationRecoveryRate");
    REQF(deformation, "pressureToDeformationScale", result.profile.deformation.pressureToDeformationScale, "deformation.pressureToDeformationScale");

    REQF(movement, "accelerationMultiplier", result.profile.movement.accelerationMultiplier, "movement.accelerationMultiplier");
    REQF(movement, "brakingMultiplier", result.profile.movement.brakingMultiplier, "movement.brakingMultiplier");
    REQF(movement, "turnControlMultiplier", result.profile.movement.turnControlMultiplier, "movement.turnControlMultiplier");
    REQF(movement, "jumpTakeoffMultiplier", result.profile.movement.jumpTakeoffMultiplier, "movement.jumpTakeoffMultiplier");
    REQF(movement, "landingStabilityMultiplier", result.profile.movement.landingStabilityMultiplier, "movement.landingStabilityMultiplier");

    REQF(feedback, "roughnessToCameraJitter", result.profile.feedback.roughnessToCameraJitter, "feedback.roughnessToCameraJitter");
    REQF(feedback, "wetnessToAudio", result.profile.feedback.wetnessToAudio, "feedback.wetnessToAudio");
    REQF(feedback, "impactToParticles", result.profile.feedback.impactToParticles, "feedback.impactToParticles");
    REQF(feedback, "slideToParticles", result.profile.feedback.slideToParticles, "feedback.slideToParticles");
    REQF(feedback, "impactToHaptics", result.profile.feedback.impactToHaptics, "feedback.impactToHaptics");

    float wetnessToFriction = 0.0F;
    float saturationToHardness = 0.0F;
    float iceToFriction = 0.0F;
    float oilToFriction = 0.0F;
    float pressureToDeformation = 0.0F;
    float impactToParticleIntensity = 0.0F;
    float slideSpeedToFeedback = 0.0F;

    REQF(curves, "wetnessToFriction", wetnessToFriction, "curves.wetnessToFriction");
    REQF(curves, "saturationToHardness", saturationToHardness, "curves.saturationToHardness");
    REQF(curves, "iceToFriction", iceToFriction, "curves.iceToFriction");
    REQF(curves, "oilToFriction", oilToFriction, "curves.oilToFriction");
    REQF(curves, "pressureToDeformation", pressureToDeformation, "curves.pressureToDeformation");
    REQF(curves, "impactToParticleIntensity", impactToParticleIntensity, "curves.impactToParticleIntensity");
    REQF(curves, "slideSpeedToFeedback", slideSpeedToFeedback, "curves.slideSpeedToFeedback");

    setFlatCurve(result.profile.curves.wetnessToFriction, wetnessToFriction);
    setFlatCurve(result.profile.curves.saturationToHardness, saturationToHardness);
    setFlatCurve(result.profile.curves.iceToFriction, iceToFriction);
    setFlatCurve(result.profile.curves.oilToFriction, oilToFriction);
    setFlatCurve(result.profile.curves.pressureToDeformation, pressureToDeformation);
    setFlatCurve(result.profile.curves.impactToParticleIntensity, impactToParticleIntensity);
    setFlatCurve(result.profile.curves.slideSpeedToFeedback, slideSpeedToFeedback);

#undef REQF
    result.ok = true;
    return result;
}

}  // namespace

SurfaceParseResult parseSurfaceProfileAuthoringJson(const std::string& jsonContent) {
    const std::string_view root = jsonContent;

    SurfaceId schemaVersion = 0;
    if (!extractUnsigned(root, "schemaVersion", schemaVersion)) {
        return {false, {}, "missing/invalid key 'schemaVersion'"};
    }

    if (schemaVersion.value == 0U) {
        return parseSurfaceProfileV0(root);
    }
    if (schemaVersion.value == 1U) {
        return parseSurfaceProfileV1(root);
    }

    return {false, {}, "unsupported schemaVersion: expected 0 or 1"};
}

SurfaceParseResult parseSurfaceProfileAuthoringFile(const std::string& filePath) {
    std::ifstream input(filePath);
    if (!input) {
        return {false, {}, "failed to open file: " + filePath};
    }

    std::stringstream buffer;
    buffer << input.rdbuf();
    return parseSurfaceProfileAuthoringJson(buffer.str());
}

}  // namespace surface
