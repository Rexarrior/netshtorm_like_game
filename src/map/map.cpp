#include "map.h"
#include "bridge_placer.h"

namespace ns {

GameMap::GameMap() = default;

void GameMap::add_island(const Island& island) {
    islands_.push_back(island);
}

bool GameMap::place_bridge(BridgeShape shape, Position origin, int owner_id) {
    auto tiles = BridgePlacer::get_occupied_tiles(shape, origin);

    if (!BridgePlacer::can_place(tiles, islands_, bridges_)) return false;
    if (!BridgePlacer::is_adjacent_to_existing(tiles, islands_, bridges_, owner_id)) return false;

    BridgeSegment bridge;
    bridge.shape = shape;
    bridge.origin = origin;
    bridge.owner_id = owner_id;
    bridge.occupied_tiles = tiles;
    bridge.hp = bridge.max_hp();

    bridges_.push_back(bridge);
    return true;
}

void GameMap::add_geyser(Position pos, int owner_id) {
    Geyser g;
    g.pos = pos;
    g.owner_id = owner_id;
    geysers_.push_back(g);
}

IslandType GameMap::tile_owner(int gx, int gy) const {
    for (const auto& island : islands_) {
        if (island.contains(gx, gy)) return island.type;
    }
    return IslandType::None;
}

bool GameMap::is_bridge_at(int gx, int gy) const {
    for (const auto& bridge : bridges_) {
        if (!bridge.is_alive()) continue;
        for (const auto& t : bridge.occupied_tiles) {
            if (t.gx == gx && t.gy == gy) return true;
        }
    }
    return false;
}

std::optional<Island> GameMap::get_island_at(int gx, int gy) const {
    for (const auto& island : islands_) {
        if (island.contains(gx, gy)) return island;
    }
    return std::nullopt;
}

int GameMap::bridge_count(int owner_id) const {
    int count = 0;
    for (const auto& b : bridges_) if (b.owner_id == owner_id) count++;
    return count;
}

int GameMap::active_bridge_count(int owner_id) const {
    int count = 0;
    for (const auto& b : bridges_) if (b.owner_id == owner_id && b.is_alive()) count++;
    return count;
}

} // namespace ns