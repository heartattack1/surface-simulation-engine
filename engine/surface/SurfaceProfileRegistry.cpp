#include "SurfaceProfileRegistry.h"

namespace surface {

bool SurfaceProfileRegistry::registerProfile(const SurfaceProfile& profile) {
    if (profile.id == kInvalidSurfaceId) {
        return false;
    }

    profiles_[profile.id] = profile;
    return true;
}

bool SurfaceProfileRegistry::loadProfiles(const std::vector<SurfaceProfile>& profiles) {
    bool allLoaded = true;
    for (const SurfaceProfile& profile : profiles) {
        allLoaded = registerProfile(profile) && allLoaded;
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
