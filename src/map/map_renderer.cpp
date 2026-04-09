#include "map_renderer.h"
#include "../core/config.h"
#include "../assets/asset_manager.h"

namespace ns {

void MapRenderer::render(const GameMap& map, const IsometricCamera2D& cam) {
    // Render islands
    for (const auto& island : map.islands()) {
        for (const auto& tile : island.tiles) {
            render_tile(tile.gx, tile.gy, island.type, cam);
        }
    }

    // Render bridges
    for (const auto& bridge : map.bridges()) {
        if (bridge.is_alive()) {
            render_bridge(bridge, cam);
        }
    }

    // Render geysers
    for (const auto& geyser : map.geysers()) {
        render_geyser(geyser, cam);
    }
}

void MapRenderer::render_tile(int gx, int gy, IslandType type, const IsometricCamera2D& cam) {
    Vector2 transformed = cam.grid_to_screen(gx, gy);

    // Try to load texture
    std::string tile_key;
    switch (type) {
        case IslandType::Home: tile_key = "tile_island_grass"; break;
        case IslandType::Resource: tile_key = "tile_island_stone"; break;
        case IslandType::Strategic: tile_key = "tile_island_grass"; break;
        case IslandType::Enemy: tile_key = "tile_island_stone"; break;
    }

    auto& am = AssetManager::instance();
    Texture2D tex = am.get_texture(tile_key);

    if (tex.id != 0) {
        Rectangle src = {0, 0, static_cast<float>(tex.width), static_cast<float>(tex.height)};
        Rectangle dst = {transformed.x - TILE_WIDTH / 2.0f, transformed.y - TILE_HEIGHT / 2.0f,
                         static_cast<float>(TILE_WIDTH), static_cast<float>(TILE_HEIGHT)};
        DrawTexturePro(tex, src, dst, {0, 0}, 0, WHITE);
    } else {
        // Fallback: draw colored diamond
        Color col = island_color(type);
        DrawTriangle(
            {transformed.x, transformed.y - TILE_HEIGHT / 2.0f},
            {transformed.x - TILE_WIDTH / 2.0f, transformed.y},
            {transformed.x + TILE_WIDTH / 2.0f, transformed.y},
            col
        );
        DrawTriangle(
            {transformed.x - TILE_WIDTH / 2.0f, transformed.y},
            {transformed.x, transformed.y + TILE_HEIGHT / 2.0f},
            {transformed.x + TILE_WIDTH / 2.0f, transformed.y},
            Fade(col, 0.8f)
        );
        // Tile thickness
        DrawTriangle(
            {transformed.x - TILE_WIDTH / 2.0f, transformed.y},
            {transformed.x, transformed.y + TILE_HEIGHT / 2.0f},
            {transformed.x, transformed.y + TILE_HEIGHT / 2.0f + TILE_DEPTH},
            Fade(col, 0.6f)
        );
        DrawTriangle(
            {transformed.x + TILE_WIDTH / 2.0f, transformed.y},
            {transformed.x, transformed.y + TILE_HEIGHT / 2.0f},
            {transformed.x, transformed.y + TILE_HEIGHT / 2.0f + TILE_DEPTH},
            Fade(col, 0.4f)
        );
    }
}

void MapRenderer::render_bridge(const BridgeSegment& bridge, const IsometricCamera2D& cam) {
    Color col = bridge_color(bridge.state);
    // Tint by owner
    if (bridge.owner_id == 0) col = ColorBrightness(col, 0.2f); // Player 1 tint
    else if (bridge.owner_id == 1) col = ColorBrightness(col, -0.2f); // Player 2 tint

    auto& am = AssetManager::instance();
    std::string shape_name;
    switch (bridge.shape) {
        case BridgeShape::Straight: shape_name = "straight"; break;
        case BridgeShape::LShape: shape_name = "l_shape"; break;
        case BridgeShape::TShape: shape_name = "t_shape"; break;
        default: shape_name = "straight"; break;
    }
    std::string state_name = bridge.state == BridgeState::Cracked ? "cracked" : "hardened";
    std::string key = "bridge_" + shape_name + "_" + state_name;

    Texture2D tex = am.get_texture(key);

    for (const auto& tile : bridge.occupied_tiles) {
        Vector2 transformed = cam.grid_to_screen(tile.gx, tile.gy);

        if (tex.id != 0) {
            Rectangle src = {0, 0, static_cast<float>(tex.width), static_cast<float>(tex.height)};
            Rectangle dst = {transformed.x - TILE_WIDTH / 4.0f, transformed.y - TILE_HEIGHT / 4.0f,
                             static_cast<float>(TILE_WIDTH / 2), static_cast<float>(TILE_HEIGHT / 2)};
            DrawTexturePro(tex, src, dst, {0, 0}, 0, col);
        } else {
            // Fallback: small rectangle
            DrawRectangle(
                static_cast<int>(transformed.x - 12),
                static_cast<int>(transformed.y - 6),
                24, 12, col
            );
        }
    }
}

void MapRenderer::render_geyser(const Geyser& geyser, const IsometricCamera2D& cam) {
    Vector2 transformed = cam.grid_to_screen(geyser.pos.x, geyser.pos.y);

    auto& am = AssetManager::instance();
    Texture2D tex = am.get_texture("geyser_storm");
    if (tex.id != 0) {
        Rectangle src = {0, 0, static_cast<float>(tex.width), static_cast<float>(tex.height)};
        Rectangle dst = {transformed.x - 16, transformed.y - 32, 32, 64};
        DrawTexturePro(tex, src, dst, {0, 0}, 0, WHITE);
    } else {
        // Fallback: glowing blue diamond
        DrawCircle(static_cast<int>(transformed.x), static_cast<int>(transformed.y - 8), 8, SKYBLUE);
        DrawCircle(static_cast<int>(transformed.x), static_cast<int>(transformed.y - 8), 4, WHITE);
    }
}

Color MapRenderer::island_color(IslandType type) const {
    switch (type) {
        case IslandType::Home: return {74, 124, 63, 255};     // Green
        case IslandType::Resource: return {139, 134, 130, 255}; // Grey
        case IslandType::Strategic: return {100, 140, 80, 255}; // Light green
        case IslandType::Enemy: return {120, 80, 80, 255};     // Reddish
    }
    return MAGENTA;
}

Color MapRenderer::bridge_color(BridgeState state) const {
    switch (state) {
        case BridgeState::Cracked: return {160, 130, 109, 255};
        case BridgeState::Hardened: return {107, 91, 79, 255};
    }
    return MAGENTA;
}

} // namespace ns
