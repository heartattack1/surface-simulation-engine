#pragma once

#include <string>
#include <vector>

#include "SurfaceProfile.h"
#include "SurfaceState.h"

namespace surface {

enum class SurfaceValidationSeverity { Warning = 0, Error };

struct SurfaceValidationError {
    SurfaceId id = kInvalidSurfaceId;
    std::string field;
    std::string message;
    SurfaceValidationSeverity severity = SurfaceValidationSeverity::Error;
};

std::vector<SurfaceValidationError> validateSurfaceProfile(const SurfaceProfile& profile);
std::vector<SurfaceValidationError> validateSurfaceState(const SurfaceState& state);

}  // namespace surface
