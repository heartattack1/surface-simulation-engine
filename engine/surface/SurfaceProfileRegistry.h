#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "SurfaceProfile.h"

namespace surface {

class SurfaceProfileRegistry {
public:
    bool registerProfile(const SurfaceProfile& profile);
    bool loadProfiles(const std::vector<SurfaceProfile>& profiles);
    bool loadFromDirectory(const std::string& directoryPath);

    const SurfaceProfile* find(SurfaceId id) const;
    std::optional<SurfaceProfile> get(SurfaceId id) const;

    std::size_t size() const;
    void clear();

private:
    std::unordered_map<SurfaceId, SurfaceProfile, std::hash<SurfaceId>> profiles_;
};

}  // namespace surface
