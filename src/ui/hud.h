#pragma once

#include <raylib.h>
#include <string>

namespace ns {

class HUD {
public:
    void render(int energy, float game_speed, bool paused) const;
};

} // namespace ns
