#pragma once

#include "../core/types.h"
#include <vector>

namespace ns {

struct Island {
    int id = 0;
    std::vector<TilePosition> tiles;
    Position center{0, 0};
    IslandType type = IslandType::Home;
    std::vector<TilePosition> connection_points;

    bool contains(int gx, int gy) const;
    TilePosition nearest_connection_point(int target_gx, int target_gy) const;
};

} // namespace ns