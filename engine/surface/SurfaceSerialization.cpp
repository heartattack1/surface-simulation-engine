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
    const std::string_view root = jsonContent;

    SurfaceId schemaVersion = 0;
    if (!extractUnsigned(root, "schemaVersion", schemaVersion) || schemaVersion != 1U) {
        result.error = "schemaVersion must be present and equal to 1";
        return result;
    }

    if (!extractUnsigned(root, "id", result.profile.id)) {
        result.error = "id must be present";
        return result;
    }

    if (!extractString(root, "name", result.profile.name)) {
        result.error = "name must be present";
        return result;
    }

    std::string categoryValue;
    if (!extractString(root, "category", categoryValue)) {
        result.error = "category must be present";
        return result;
    }
    result.profile.category = categoryFromString(categoryValue);

    std::string_view physical;
    std::string_view environmental;
    std::string_view deformation;
    std::string_view movement;
    if (!extractObject(root, "physical", physical) ||
        !extractObject(root, "environmental", environmental) ||
        !extractObject(root, "deformation", deformation) ||
        !extractObject(root, "movement", movement)) {
        result.error = "missing one or more required objects";
        return result;
    }

    if (!extractFloat(physical, "friction", result.profile.physical.friction) ||
        !extractFloat(physical, "staticFriction", result.profile.physical.staticFriction) ||
        !extractFloat(physical, "restitution", result.profile.physical.restitution) ||
        !extractFloat(physical, "roughness", result.profile.physical.roughness) ||
        !extractFloat(environmental, "thermalConductivity", result.profile.environmental.thermalConductivity) ||
        !extractFloat(environmental, "heatCapacity", result.profile.environmental.heatCapacity) ||
        !extractFloat(environmental, "absorbency", result.profile.environmental.absorbency) ||
        !extractFloat(deformation, "softness", result.profile.deformation.softness) ||
        !extractFloat(deformation, "maxCompression", result.profile.deformation.maxCompression) ||
        !extractFloat(deformation, "recoveryRate", result.profile.deformation.recoveryRate) ||
        !extractFloat(movement, "speedMultiplier", result.profile.movement.speedMultiplier) ||
        !extractFloat(movement, "accelerationMultiplier", result.profile.movement.accelerationMultiplier) ||
        !extractFloat(movement, "brakingMultiplier", result.profile.movement.brakingMultiplier) ||
        !extractFloat(movement, "lateralControlMultiplier", result.profile.movement.lateralControlMultiplier)) {
        result.error = "one or more required nested properties are missing or malformed";
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
