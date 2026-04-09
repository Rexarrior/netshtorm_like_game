#include "timer.h"

namespace ns {

GameTimer::GameTimer() = default;

void GameTimer::update(float dt) {
    if (!paused_) {
        elapsed_ += dt * speed_;
    }
}

void GameTimer::set_speed(float speed) { speed_ = speed; }
float GameTimer::get_speed() const { return speed_; }
void GameTimer::toggle_pause() { paused_ = !paused_; }
bool GameTimer::is_paused() const { return paused_; }
float GameTimer::get_elapsed() const { return elapsed_; }

} // namespace ns
