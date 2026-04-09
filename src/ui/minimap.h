#pragma once

#include <raylib.h>
#include "../map/map.h"

namespace ns {

class Minimap {
public:
    Minimap(int x, int y, int size);
    void render(const GameMap& map, class GameCamera& game_cam);

private:
    int x_, y_, size_;
};

} // namespace ns
