#include "island_generator.h"

namespace ns {

Island IslandGenerator::generate_diamond_island(int cx, int cy, int radius, int id, IslandType type) {
    Island island;
    island.id = id;
    island.type = type;
    island.center = {cx, cy};

    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            if (std::abs(dx) + std::abs(dy) <= radius) {
                island.tiles.push_back({cx + dx, cy + dy});
            }
        }
    }

    // Connection points: edges of the diamond
    // Top, Bottom, Left, Right
    island.connection_points.push_back({cx, cy - radius});      // North
    island.connection_points.push_back({cx, cy + radius});      // South
    island.connection_points.push_back({cx - radius, cy});      // West
    island.connection_points.push_back({cx + radius, cy});      // East

    return island;
}

std::vector<Island> IslandGenerator::generate_home_island(int cx, int cy, int radius) {
    return {generate_diamond_island(cx, cy, radius, 0, IslandType::Home)};
}

std::vector<Island> IslandGenerator::generate_resource_island(int cx, int cy, int radius, int id) {
    return {generate_diamond_island(cx, cy, radius, id, IslandType::Resource)};
}

std::vector<Island> IslandGenerator::generate_enemy_island(int cx, int cy, int radius, int id) {
    return {generate_diamond_island(cx, cy, radius, id, IslandType::Enemy)};
}

std::vector<Island> IslandGenerator::generate_strategic_island(int cx, int cy, int radius, int id) {
    return {generate_diamond_island(cx, cy, radius, id, IslandType::Strategic)};
}

} // namespace ns