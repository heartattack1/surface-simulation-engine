#pragma once

#include <string>

#include "SurfaceProfile.h"

namespace surface {

struct SurfaceParseResult {
    bool ok = false;
    SurfaceProfile profile;
    std::string error;
};

SurfaceParseResult parseSurfaceProfileAuthoringJson(const std::string& jsonContent);
SurfaceParseResult parseSurfaceProfileAuthoringFile(const std::string& filePath);

}  // namespace surface
