#pragma once

#include "island.h"
#include "bridge.h"
#include "geyser.h"
#include <vector>
#include <unordered_map>
#include <optional>

namespace ns {

class GameMap {
public:
    GameMap();

    void add_island(const Island& island);
    bool place_bridge(BridgeShape shape, Position origin, int owner_id);
    void add_geyser(Position pos, int owner_id);

    const std::vector<Island>& islands() const { return islands_; }
    const std::vector<BridgeSegment>& bridges() const { return bridges_; }
    const std::vector<Geyser>& geysers() const { return geysers_; }

    IslandType tile_owner(int gx, int gy) const;
    bool is_bridge_at(int gx, int gy) const;
    std::optional<Island> get_island_at(int gx, int gy) const;

    int bridge_count(int owner_id) const;
    int active_bridge_count(int owner_id) const;

private:
    std::vector<Island> islands_;
    std::vector<BridgeSegment> bridges_;
    std::vector<Geyser> geysers_;
};

} // namespace ns