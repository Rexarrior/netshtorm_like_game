#pragma once

#include "core/types.h"
#include "core/random.h"
#include "core/timer.h"
#include "core/state_machine.h"
#include "assets/asset_manager.h"
#include "assets/sprite_sheet.h"
#include "assets/sound_bank.h"
#include "input/input_handler.h"
#include "camera/game_camera.h"
#include "camera/minimap_camera.h"
#include "map/map.h"
#include "map/map_renderer.h"
#include "map/island_generator.h"
#include "map/geyser.h"
#include "ui/menu.h"
#include "ui/hud.h"
#include "ui/minimap.h"
#include "systems/particle_system.h"
#include <memory>
#include <vector>

namespace ns {

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void init_demo_map();
    void update();
    void render();
    void handle_input();

    GameCamera camera_;
    MinimapCamera minimap_cam_;
    InputHandler input_;
    MapRenderer map_renderer_;
    Menu menu_;
    HUD hud_;
    std::unique_ptr<Minimap> minimap_;
    ParticleSystem particles_;
    GameTimer timer_;
    StateMachine state_machine_;

    std::unique_ptr<GameMap> map_;
    int energy_ = 500;
    int current_player_ = 0;
};

} // namespace ns