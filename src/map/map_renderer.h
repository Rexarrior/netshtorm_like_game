#pragma once

#include <raylib.h>
#include "map.h"
#include "../camera/isometric_camera.h"
#include <string>

namespace ns {

class MapRenderer {
public:
    void render(const GameMap& map, const IsometricCamera2D& cam);
    void render_tile(int gx, int gy, IslandType type, const IsometricCamera2D& cam);
    void render_bridge(const BridgeSegment& bridge, const IsometricCamera2D& cam);
    void render_geyser(const Geyser& geyser, const IsometricCamera2D& cam);

    // Fallback colors when no textures
    Color island_color(IslandType type) const;
    Color bridge_color(BridgeState state) const;
};

} // namespace ns
