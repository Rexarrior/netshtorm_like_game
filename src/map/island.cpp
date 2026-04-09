#include "island.h"
#include <limits>
#include <cmath>

namespace ns {

bool Island::contains(int gx, int gy) const {
    for (const auto& t : tiles) {
        if (t.gx == gx && t.gy == gy) return true;
    }
    return false;
}

TilePosition Island::nearest_connection_point(int target_gx, int target_gy) const {
    TilePosition best = connection_points.empty() ? TilePosition{center.x, center.y} : connection_points[0];
    int best_dist = std::numeric_limits<int>::max();
    for (const auto& cp : connection_points) {
        int dist = std::abs(cp.gx - target_gx) + std::abs(cp.gy - target_gy);
        if (dist < best_dist) {
            best_dist = dist;
            best = cp;
        }
    }
    return best;
}

} // namespace ns