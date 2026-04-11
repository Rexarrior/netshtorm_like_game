#include "game.h"
#include "core/config.h"
#include "map/bridge_placer.h"
#include <iostream>
#include <algorithm>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace ns {

Game::Game() : minimap_cam_(32) {
    // Initialize test controller if GAME_TEST_MODE is set
    TestController::instance().init();
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "NetStorm-Like");
    SetTargetFPS(TARGET_FPS);

    std::string asset_path = "assets";
    AssetManager::instance().load_textures_from_dir(asset_path + "/tiles", "tile");
    AssetManager::instance().load_textures_from_dir(asset_path + "/bridges", "bridge");
    AssetManager::instance().load_textures_from_dir(asset_path + "/units", "unit");
    AssetManager::instance().load_textures_from_dir(asset_path + "/buildings", "building");
    AssetManager::instance().load_textures_from_dir(asset_path + "/geysers", "geyser");
    AssetManager::instance().load_textures_from_dir(asset_path + "/backgrounds", "bg");
    AssetManager::instance().load_textures_from_dir(asset_path + "/ui", "ui");

    std::cerr << "[Game] Asset loading complete." << std::endl;

    state_machine_.set_state(GameState::Menu);

    menu_.set_on_start([this]() {
        state_machine_.set_state(GameState::Playing);
        init_demo_map();
    });
    menu_.set_on_resume([this]() {
        state_machine_.set_state(GameState::Playing);
    });
    menu_.set_on_quit([this]() {
        CloseWindow();
    });

    minimap_ = std::make_unique<Minimap>(SCREEN_WIDTH - 180, 50, 160);
}

Game::~Game() {
    AssetManager::instance().unload_all();
    CloseWindow();
}

void Game::start_game() {
    state_machine_.set_state(GameState::Playing);
    init_demo_map();
    std::cerr << "[Game] Game started from test controller" << std::endl;
}

void Game::init_demo_map() {
    map_ = std::make_unique<GameMap>();

    auto home = IslandGenerator::generate_home_island(4, 4, 2);
    for (const auto& island : home) map_->add_island(island);

    auto resource = IslandGenerator::generate_resource_island(12, 6, 2, 1);
    for (const auto& island : resource) map_->add_island(island);

    auto strategic = IslandGenerator::generate_strategic_island(8, 10, 1, 2);
    for (const auto& island : strategic) map_->add_island(island);

    auto enemy = IslandGenerator::generate_enemy_island(16, 14, 2, 3);
    for (const auto& island : enemy) map_->add_island(island);

    map_->add_geyser({12, 6}, 0);
    map_->add_geyser({16, 14}, 1);

    camera_.follow({4, 4});
    camera_.set_zoom(1.5f);  // Close-up view of home island
    camera_.snap_to_target(); // Instantly move visual to target

    energy_ = 500;
    std::cerr << "[Game] Demo map initialized with " << map_->islands().size() << " islands." << std::endl;
}

void Game::handle_input() {
    input_.update();

    if (state_machine_.get_state() == GameState::Menu) {
        if (input_.is_action_just_pressed(InputAction::Escape)) {
            CloseWindow();
        }
        return;
    }

    if (state_machine_.get_state() == GameState::Paused) {
        if (input_.is_action_just_pressed(InputAction::Pause) ||
            input_.is_action_just_pressed(InputAction::Escape)) {
            state_machine_.set_state(GameState::Playing);
        }
        return;
    }

    if (state_machine_.get_state() == GameState::GameOver || state_machine_.get_state() == GameState::Victory) {
        if (input_.is_action_just_pressed(InputAction::Enter)) {
            state_machine_.set_state(GameState::Menu);
        }
        return;
    }

    if (state_machine_.get_state() == GameState::Playing) {
        if (input_.is_action_just_pressed(InputAction::Pause) ||
            input_.is_action_just_pressed(InputAction::Escape)) {
            state_machine_.set_state(GameState::Paused);
            return;
        }

        if (input_.is_action_just_pressed(InputAction::SpeedUp)) {
            timer_.set_speed(std::min(timer_.get_speed() + 1.0f, 4.0f));
        }
        if (input_.is_action_just_pressed(InputAction::SpeedDown)) {
            timer_.set_speed(std::max(timer_.get_speed() - 1.0f, 1.0f));
        }

        // Bridge shape selection with number keys
        if (IsKeyPressed(KEY_ONE)) selected_bridge_shape_ = BridgeShape::Straight;
        if (IsKeyPressed(KEY_TWO)) selected_bridge_shape_ = BridgeShape::LShape;
        if (IsKeyPressed(KEY_THREE)) selected_bridge_shape_ = BridgeShape::TShape;
        if (IsKeyPressed(KEY_FOUR)) selected_bridge_shape_ = BridgeShape::Cross;

        // Track hover position for bridge preview
        if (map_) {
            hover_grid_pos_ = camera_.screen_to_grid(input_.get_mouse_position());
            auto tiles = BridgePlacer::get_occupied_tiles(selected_bridge_shape_, hover_grid_pos_);
            bridge_placement_valid_ = BridgePlacer::can_place(tiles, map_->islands(), map_->bridges()) &&
                                     BridgePlacer::is_adjacent_to_existing(tiles, map_->islands(), map_->bridges(), current_player_);
            show_bridge_preview_ = true;
        }

        if (input_.is_mouse_left_just_pressed() && map_) {
            Position grid = camera_.screen_to_grid(input_.get_mouse_position());
            if (energy_ >= 50) {
                bool placed = map_->place_bridge(selected_bridge_shape_, grid, current_player_);
                if (placed) {
                    energy_ -= 50;
                    std::cerr << "[Game] Bridge placed at (" << grid.x << "," << grid.y << ")" << std::endl;
                }
            }
        }
    }
}

void Game::update() {
    float dt = GetFrameTime();
    timer_.update(dt);
    input_.update();
    camera_.update(dt, input_);
    particles_.update(dt);
    handle_input();

    if (state_machine_.get_state() == GameState::Playing && map_) {
        for (const auto& geyser : map_->geysers()) {
            if (geyser.owner_id == current_player_ && geyser.active) {
                energy_ += static_cast<int>(geyser.production_rate * dt * timer_.get_speed());
            }
        }
    }
}

void Game::render_scene() {
    // Renders the current game scene - used for both screen and offscreen rendering
    ClearBackground({26, 26, 46, 255});

    if (state_machine_.get_state() == GameState::Menu) {
        menu_.render_main_menu();
        return;
    }

    if (state_machine_.get_state() == GameState::Paused) {
        if (map_) map_renderer_.render(*map_, camera_);
        menu_.render_paused();
        return;
    }

    if (state_machine_.get_state() == GameState::GameOver || state_machine_.get_state() == GameState::Victory) {
        if (map_) map_renderer_.render(*map_, camera_);
        menu_.render_game_over(state_machine_.get_state() == GameState::Victory, 1, 1);
        return;
    }

    // No BeginMode2D - IsometricCamera2D handles all coordinate transformation manually
    if (map_) {
        map_renderer_.render(*map_, camera_);
    }

    // Render bridge placement preview
    if (show_bridge_preview_ && map_) {
        auto tiles = BridgePlacer::get_occupied_tiles(selected_bridge_shape_, hover_grid_pos_);
        Color preview_col = bridge_placement_valid_ ? Color{0, 255, 0, 128} : Color{255, 0, 0, 128};
        for (const auto& tile : tiles) {
            Vector2 pos = camera_.grid_to_screen(tile.gx, tile.gy);
            DrawCircle(static_cast<int>(pos.x), static_cast<int>(pos.y), 8, preview_col);
        }
        show_bridge_preview_ = false; // Reset after render
    }

    particles_.render();

    hud_.render(energy_, timer_.get_speed(), timer_.is_paused());

    if (map_) {
        minimap_->render(*map_, camera_);
    }

    std::string debug = "Player: " + std::to_string(current_player_) +
                        " | Bridges: " + std::to_string(map_ ? map_->active_bridge_count(current_player_) : 0);
    DrawText(debug.c_str(), 10, SCREEN_HEIGHT - 30, 14, GRAY);
    DrawText("Click to place bridges", 10, SCREEN_HEIGHT - 14, 14, GRAY);
}

void Game::take_offscreen_screenshot(const std::string& name) {
#ifdef ENABLE_SCREENSHOT_CAPTURE
    // Create offscreen render texture if not exists
    if (!screenshot_texture_ || screenshot_texture_->texture.width != SCREEN_WIDTH || 
        screenshot_texture_->texture.height != SCREEN_HEIGHT) {
        screenshot_texture_ = std::make_unique<RenderTexture2D>(
            LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT));
        std::cerr << "[Game] Created offscreen texture: " << SCREEN_WIDTH << "x" << SCREEN_HEIGHT << std::endl;
    }
    
    // Ensure screenshots directory exists
    #ifdef _WIN32
    if (_mkdir("test_screenshots") != 0 && errno != 0 && errno != EEXIST) {
    #else
    struct stat st;
    if (stat("test_screenshots", &st) != 0) {
        if (mkdir("test_screenshots", 0755) != 0 && errno != EEXIST) {
    #endif
    }
    
    std::string filepath = std::string("test_screenshots/") + name + ".png";
    std::cerr << "[Game] Taking offscreen screenshot: " << filepath << std::endl;
    
    // Render to offscreen texture
    BeginTextureMode(*screenshot_texture_);
        render_scene();
    EndTextureMode();
    
    // Export the texture to file
    // Note: RenderTexture has texture.id, width, height
    Image img = LoadImageFromTexture(screenshot_texture_->texture);
    
    // Fix for macOS: RenderTexture Y-axis is flipped on some macOS GPUs
    // Flipping vertically corrects the orientation for screenshots
    ImageFlipVertical(&img);
    
    ExportImage(img, filepath.c_str());
    UnloadImage(img);
    
    std::cerr << "[Game] Screenshot saved: " << filepath << std::endl;
#else
    std::cerr << "[Game] Screenshot disabled (ENABLE_SCREENSHOT_CAPTURE not defined)" << std::endl;
#endif
}

void Game::render() {
    BeginDrawing();
    render_scene();
    EndDrawing();
    
    // Check if screenshot was requested and take offscreen screenshot
    if (TestController::instance().has_pending_screenshot()) {
        std::string name = TestController::instance().pending_screenshot_name();
        take_offscreen_screenshot(name);
        TestController::instance().clear_pending_screenshot();
    }
}

void Game::run() {
    while (!WindowShouldClose()) {
        // Process test controller commands if in test mode
        TestController::instance().process_commands(*this);
        
        update();
        render();
    }
}

void Game::pan_camera(int dx, int dy) {
    camera_.pan_by_delta(static_cast<float>(dx), static_cast<float>(dy));
}

void Game::zoom_camera(float factor) {
    float new_zoom = camera_.get_zoom() * factor;
    camera_.set_target_zoom(new_zoom);
}

} // namespace ns