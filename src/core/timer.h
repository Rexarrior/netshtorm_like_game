#pragma once

namespace ns {

class GameTimer {
public:
    GameTimer();
    void update(float dt);
    void set_speed(float speed);
    float get_speed() const;
    void toggle_pause();
    bool is_paused() const;
    float get_elapsed() const;

private:
    float elapsed_ = 0.0f;
    float speed_ = 1.0f;
    bool paused_ = false;
};

} // namespace ns
