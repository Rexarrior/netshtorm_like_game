#pragma once

#include "island.h"
#include <vector>

namespace ns {

class IslandGenerator {
public:
    static std::vector<Island> generate_home_island(int cx, int cy, int radius);
    static std::vector<Island> generate_resource_island(int cx, int cy, int radius, int id);
    static std::vector<Island> generate_enemy_island(int cx, int cy, int radius, int id);
    static std::vector<Island> generate_strategic_island(int cx, int cy, int radius, int id);
    static Island generate_diamond_island(int cx, int cy, int radius, int id, IslandType type);
};

} // namespace ns