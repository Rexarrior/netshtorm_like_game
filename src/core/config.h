#pragma once

namespace ns {

// Screen
constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr int TARGET_FPS = 60;

// Isometric tiles
constexpr int TILE_WIDTH = 64;
constexpr int TILE_HEIGHT = 32;
constexpr int TILE_DEPTH = 16;

// Map
constexpr int MAP_GRID_SIZE = 32;

// Camera
constexpr float CAMERA_MIN_ZOOM = 0.5f;
constexpr float CAMERA_MAX_ZOOM = 2.0f;
constexpr float CAMERA_PAN_SPEED = 300.0f;
constexpr float CAMERA_ZOOM_SPEED = 0.1f;

// Colors (element palettes)
// Sun: Gold #FFD700, Orange #FFA500, Yellow #FFFF00
// Thunder: Indigo #4B0082, Deep Sky Blue #00BFFF, Lightning Yellow #FFFF00

} // namespace ns
