#pragma once

#include "../core/types.h"

namespace ns {

struct Geyser {
    Position pos{0, 0};
    int production_rate = 10;  // Energy per second
    bool active = true;
    int owner_id = -1;
};

} // namespace ns