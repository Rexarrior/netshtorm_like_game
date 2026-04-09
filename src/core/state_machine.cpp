#include "state_machine.h"

namespace ns {

void StateMachine::set_state(GameState state) {
    if (current_state_ != state) {
        auto it = on_exit_callbacks_.find(current_state_);
        if (it != on_exit_callbacks_.end()) it->second();
        current_state_ = state;
        auto it2 = on_enter_callbacks_.find(current_state_);
        if (it2 != on_enter_callbacks_.end()) it2->second();
    }
}

GameState StateMachine::get_state() const { return current_state_; }

void StateMachine::on_enter(GameState state, StateCallback cb) {
    on_enter_callbacks_[state] = std::move(cb);
}

void StateMachine::on_exit(GameState state, StateCallback cb) {
    on_exit_callbacks_[state] = std::move(cb);
}

} // namespace ns
