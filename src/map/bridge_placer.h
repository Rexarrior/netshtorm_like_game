#pragma once

#include "bridge.h"
#include "island.h"
#include "../core/types.h"
#include <vector>
#include <unordered_set>

namespace ns {

class BridgePlacer {
public:
    static std::vector<TilePosition> get_occupied_tiles(BridgeShape shape, Position origin);
    static bool can_place(const std::vector<TilePosition>& tiles,
                          const std::vector<Island>& islands,
                          const std::vector<BridgeSegment>& existing_bridges);
    static bool is_connected_to_owner(const std::vector<TilePosition>& tiles,
                                      int owner_id,
                                      const std::vector<Island>& islands,
                                      const std::vector<BridgeSegment>& existing_bridges);
    static bool is_adjacent_to_existing(const std::vector<TilePosition>& new_tiles,
                                        const std::vector<Island>& islands,
                                        const std::vector<BridgeSegment>& existing_bridges,
                                        int owner_id);
};

} // namespace ns