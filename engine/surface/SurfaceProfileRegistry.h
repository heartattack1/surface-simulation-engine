#pragma once

#include <optional>
#include <unordered_map>
#include <vector>

#include "SurfaceProfile.h"

namespace surface {

class SurfaceProfileRegistry {
public:
    bool registerProfile(const SurfaceProfile& profile);
    bool loadProfiles(const std::vector<SurfaceProfile>& profiles);

    const SurfaceProfile* find(SurfaceId id) const;
    std::optional<SurfaceProfile> get(SurfaceId id) const;

    std::size_t size() const;
    void clear();

private:
    std::unordered_map<SurfaceId, SurfaceProfile> profiles_;
};

}  // namespace surface
