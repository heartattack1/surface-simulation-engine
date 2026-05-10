#include "SurfaceProfileRegistry.h"

#include <filesystem>

#include "SurfaceSerialization.h"
#include "SurfaceValidation.h"

namespace surface {

bool SurfaceProfileRegistry::registerProfile(const SurfaceProfile& profile) {
    if (profile.id == kInvalidSurfaceId) {
        return false;
    }

    const std::vector<SurfaceValidationError> validationErrors = validateSurfaceProfile(profile);
    if (!validationErrors.empty()) {
        return false;
    }

    const auto [_, inserted] = profiles_.emplace(profile.id, profile);
    if (!inserted) {
        return false;
    }

    return true;
}

bool SurfaceProfileRegistry::loadProfiles(const std::vector<SurfaceProfile>& profiles) {
    bool allLoaded = true;
    for (const SurfaceProfile& profile : profiles) {
        allLoaded = registerProfile(profile) && allLoaded;
    }
    return allLoaded;
}

bool SurfaceProfileRegistry::loadFromDirectory(const std::string& directoryPath) {
    bool allLoaded = true;

    if (!std::filesystem::exists(directoryPath)) {
        return false;
    }

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".json" ||
            entry.path().stem().extension() != ".surface") {
            continue;
        }

        const SurfaceParseResult parsed = parseSurfaceProfileAuthoringFile(entry.path().string());
        if (!parsed.ok || parsed.profile.category == SurfaceCategory::Unknown || !registerProfile(parsed.profile)) {
            allLoaded = false;
            continue;
        }
    }

    return allLoaded;
}

const SurfaceProfile* SurfaceProfileRegistry::find(SurfaceId id) const {
    const auto it = profiles_.find(id);
    if (it == profiles_.end()) {
        return nullptr;
    }
    return &it->second;
}

std::optional<SurfaceProfile> SurfaceProfileRegistry::get(SurfaceId id) const {
    const SurfaceProfile* found = find(id);
    if (found == nullptr) {
        return std::nullopt;
    }
    return *found;
}

std::size_t SurfaceProfileRegistry::size() const {
    return profiles_.size();
}

void SurfaceProfileRegistry::clear() {
    profiles_.clear();
}

}  // namespace surface
