#pragma once

#include "types.h"
#include <functional>
#include <map>

namespace ns {

class StateMachine {
public:
    using StateCallback = std::function<void()>;

    void set_state(GameState state);
    GameState get_state() const;
    void on_enter(GameState state, StateCallback cb);
    void on_exit(GameState state, StateCallback cb);

private:
    GameState current_state_ = GameState::Menu;
    std::map<GameState, StateCallback> on_enter_callbacks_;
    std::map<GameState, StateCallback> on_exit_callbacks_;
};

} // namespace ns
