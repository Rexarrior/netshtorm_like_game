#include "bridge_placer.h"
#include "../core/config.h"
#include <unordered_set>

namespace ns {

std::vector<TilePosition> BridgePlacer::get_occupied_tiles(BridgeShape shape, Position origin) {
    std::vector<TilePosition> tiles;
    switch (shape) {
        case BridgeShape::Straight:
            tiles.push_back({origin.x, origin.y});
            break;
        case BridgeShape::LShape:
            tiles.push_back({origin.x, origin.y});
            tiles.push_back({origin.x + 1, origin.y});
            tiles.push_back({origin.x, origin.y + 1});
            break;
        case BridgeShape::TShape:
            tiles.push_back({origin.x, origin.y});
            tiles.push_back({origin.x + 1, origin.y});
            tiles.push_back({origin.x - 1, origin.y});
            tiles.push_back({origin.x, origin.y + 1});
            break;
        case BridgeShape::Cross:
            tiles.push_back({origin.x, origin.y});
            tiles.push_back({origin.x + 1, origin.y});
            tiles.push_back({origin.x - 1, origin.y});
            tiles.push_back({origin.x, origin.y + 1});
            tiles.push_back({origin.x, origin.y - 1});
            break;
        case BridgeShape::LongStraight:
            tiles.push_back({origin.x, origin.y});
            tiles.push_back({origin.x + 1, origin.y});
            break;
        case BridgeShape::Zigzag:
            tiles.push_back({origin.x, origin.y});
            tiles.push_back({origin.x + 1, origin.y + 1});
            break;
    }
    return tiles;
}

bool BridgePlacer::can_place(const std::vector<TilePosition>& tiles,
                              const std::vector<Island>& islands,
                              const std::vector<BridgeSegment>& existing_bridges) {
    (void)islands;
    std::unordered_set<long> occupied;
    for (const auto& bridge : existing_bridges) {
        if (!bridge.is_alive()) continue;
        for (const auto& t : bridge.occupied_tiles) {
            occupied.insert(static_cast<long>(t.gx) * 10000 + t.gy);
        }
    }
    for (const auto& t : tiles) {
        if (occupied.count(static_cast<long>(t.gx) * 10000 + t.gy)) return false;
    }
    return true;
}

bool BridgePlacer::is_connected_to_owner(const std::vector<TilePosition>& tiles,
                                          int owner_id,
                                          const std::vector<Island>& islands,
                                          const std::vector<BridgeSegment>& existing_bridges) {
    std::unordered_set<long> visited;
    std::vector<TilePosition> queue;

    for (const auto& t : tiles) {
        long key = static_cast<long>(t.gx) * 10000 + t.gy;
        if (!visited.count(key)) {
            visited.insert(key);
            queue.push_back(t);
        }
    }

    size_t head = 0;
    const int max_range = MAP_GRID_SIZE;
    while (head < queue.size()) {
        TilePosition current = queue[head++];

        // Bounds check to prevent unbounded BFS
        if (current.gx < -max_range || current.gx > max_range ||
            current.gy < -max_range || current.gy > max_range) {
            continue;
        }

        for (const auto& island : islands) {
            if (island.type == IslandType::Enemy && owner_id == 0) continue;
            if (island.type == IslandType::Home && owner_id == 1) continue;
            if (island.contains(current.gx, current.gy)) return true;
        }

        for (const auto& bridge : existing_bridges) {
            if (!bridge.is_alive() || bridge.owner_id != owner_id) continue;
            for (const auto& bt : bridge.occupied_tiles) {
                if (std::abs(bt.gx - current.gx) + std::abs(bt.gy - current.gy) <= 1) {
                    return true;
                }
            }
        }

        int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
        for (auto& d : dirs) {
            TilePosition next{current.gx + d[0], current.gy + d[1]};
            long key = static_cast<long>(next.gx) * 10000 + next.gy;
            if (!visited.count(key)) {
                visited.insert(key);
                queue.push_back(next);
            }
        }
    }
    return false;
}

bool BridgePlacer::is_adjacent_to_existing(const std::vector<TilePosition>& new_tiles,
                                            const std::vector<Island>& islands,
                                            const std::vector<BridgeSegment>& existing_bridges,
                                            int owner_id) {
    for (const auto& nt : new_tiles) {
        for (const auto& island : islands) {
            if (island.type == IslandType::Enemy && owner_id == 0) continue;
            if (island.type == IslandType::Home && owner_id == 1) continue;
            for (const auto& cp : island.connection_points) {
                if (std::abs(cp.gx - nt.gx) + std::abs(cp.gy - nt.gy) <= 1) return true;
            }
        }
        for (const auto& bridge : existing_bridges) {
            if (!bridge.is_alive() || bridge.owner_id != owner_id) continue;
            for (const auto& bt : bridge.occupied_tiles) {
                if (std::abs(bt.gx - nt.gx) + std::abs(bt.gy - nt.gy) <= 1) return true;
            }
        }
    }
    return false;
}

} // namespace ns
