#pragma once

#include "../core/types.h"
#include <vector>

namespace ns {

struct BridgeSegment {
    BridgeShape shape = BridgeShape::Straight;
    BridgeState state = BridgeState::Cracked;
    int hp = 50;
    Position origin{0, 0};
    int owner_id = -1;
    std::vector<TilePosition> occupied_tiles;

    int max_hp() const {
        return state == BridgeState::Hardened ? 150 : 50;
    }

    void take_damage(int amount) {
        hp -= amount;
        if (hp < 0) hp = 0;
    }

    bool is_alive() const { return hp > 0; }

    void harden() {
        state = BridgeState::Hardened;
        hp = max_hp();
    }
};

} // namespace ns