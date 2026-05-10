#include "SurfaceSerialization.h"

#include <fstream>
#include <regex>
#include <sstream>

namespace surface {

namespace {

bool extractString(const std::string& src, const std::string& key, std::string& out) {
    const std::regex pattern("\"" + key + "\"\\s*:\\s*\"([^\"]+)\"");
    std::smatch match;
    if (!std::regex_search(src, match, pattern)) {
        return false;
    }
    out = match[1].str();
    return true;
}

bool extractUnsigned(const std::string& src, const std::string& key, SurfaceId& out) {
    const std::regex pattern("\"" + key + "\"\\s*:\\s*(\\d+)");
    std::smatch match;
    if (!std::regex_search(src, match, pattern)) {
        return false;
    }
    out = static_cast<SurfaceId>(std::stoul(match[1].str()));
    return true;
}

bool extractFloat(const std::string& src, const std::string& key, float& out) {
    const std::regex pattern("\"" + key + "\"\\s*:\\s*(-?\\d+(?:\\.\\d+)?)");
    std::smatch match;
    if (!std::regex_search(src, match, pattern)) {
        return false;
    }
    out = std::stof(match[1].str());
    return true;
}

SurfaceCategory categoryFromString(const std::string& category) {
    if (category == "Hard") return SurfaceCategory::Hard;
    if (category == "Loose") return SurfaceCategory::Loose;
    if (category == "Liquid") return SurfaceCategory::Liquid;
    if (category == "Organic") return SurfaceCategory::Organic;
    if (category == "Synthetic") return SurfaceCategory::Synthetic;
    return SurfaceCategory::Unknown;
}

}  // namespace

SurfaceParseResult parseSurfaceProfileAuthoringJson(const std::string& jsonContent) {
    SurfaceParseResult result;

    SurfaceId schemaVersion = 0;
    if (!extractUnsigned(jsonContent, "schemaVersion", schemaVersion) || schemaVersion != 1U) {
        result.error = "schemaVersion must be present and equal to 1";
        return result;
    }

    if (!extractUnsigned(jsonContent, "id", result.profile.id)) {
        result.error = "id must be present";
        return result;
    }

    std::string categoryValue;
    if (!extractString(jsonContent, "category", categoryValue)) {
        result.error = "category must be present";
        return result;
    }

    result.profile.name = categoryValue;
    result.profile.category = categoryFromString(categoryValue);

    if (!extractFloat(jsonContent, "friction", result.profile.physical.friction) ||
        !extractFloat(jsonContent, "staticFriction", result.profile.physical.staticFriction) ||
        !extractFloat(jsonContent, "restitution", result.profile.physical.restitution) ||
        !extractFloat(jsonContent, "roughness", result.profile.physical.roughness) ||
        !extractFloat(jsonContent,
                      "thermalConductivity",
                      result.profile.environmental.thermalConductivity) ||
        !extractFloat(jsonContent, "heatCapacity", result.profile.environmental.heatCapacity) ||
        !extractFloat(jsonContent, "absorbency", result.profile.environmental.absorbency) ||
        !extractFloat(jsonContent, "softness", result.profile.deformation.softness) ||
        !extractFloat(jsonContent, "maxCompression", result.profile.deformation.maxCompression) ||
        !extractFloat(jsonContent, "recoveryRate", result.profile.deformation.recoveryRate) ||
        !extractFloat(jsonContent, "speedMultiplier", result.profile.movement.speedMultiplier) ||
        !extractFloat(jsonContent,
                      "accelerationMultiplier",
                      result.profile.movement.accelerationMultiplier) ||
        !extractFloat(jsonContent, "brakingMultiplier", result.profile.movement.brakingMultiplier) ||
        !extractFloat(jsonContent,
                      "lateralControlMultiplier",
                      result.profile.movement.lateralControlMultiplier)) {
        result.error = "one or more required nested properties are missing";
        return result;
    }

    result.ok = true;
    return result;
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
