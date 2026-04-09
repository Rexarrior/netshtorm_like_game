#include "input_handler.h"

namespace ns {

void InputHandler::update() {
    pressed_actions_.clear();
    just_pressed_actions_.clear();

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) pressed_actions_.push_back(InputAction::PanUp);
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) pressed_actions_.push_back(InputAction::PanDown);
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) pressed_actions_.push_back(InputAction::PanLeft);
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) pressed_actions_.push_back(InputAction::PanRight);
    if (IsKeyDown(KEY_EQUAL) || IsKeyDown(KEY_KP_ADD)) pressed_actions_.push_back(InputAction::SpeedUp);
    if (IsKeyDown(KEY_MINUS) || IsKeyDown(KEY_KP_SUBTRACT)) pressed_actions_.push_back(InputAction::SpeedDown);
    if (IsKeyPressed(KEY_SPACE)) pressed_actions_.push_back(InputAction::Pause);
    if (IsKeyPressed(KEY_ENTER)) pressed_actions_.push_back(InputAction::Enter);
    if (IsKeyPressed(KEY_ESCAPE)) pressed_actions_.push_back(InputAction::Escape);

    // Mouse
    mouse_pos_ = GetMousePosition();
    wheel_move_ = GetMouseWheelMove();

    bool mouse_left_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    if (mouse_left_down && !mouse_left_was_down_) {
        just_pressed_actions_.push_back(InputAction::MouseLeft);
    }
    if (mouse_left_down) {
        pressed_actions_.push_back(InputAction::MouseLeft);
    }
    mouse_left_was_down_ = mouse_left_down;

    bool mouse_right_down = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    if (mouse_right_down) pressed_actions_.push_back(InputAction::MouseRight);
}

bool InputHandler::is_action_pressed(InputAction action) const {
    for (auto a : pressed_actions_) if (a == action) return true;
    return false;
}

bool InputHandler::is_action_just_pressed(InputAction action) const {
    for (auto a : just_pressed_actions_) if (a == action) return true;
    return false;
}

Vector2 InputHandler::get_mouse_position() const { return mouse_pos_; }
bool InputHandler::is_mouse_left_pressed() const { return is_action_pressed(InputAction::MouseLeft); }
bool InputHandler::is_mouse_left_just_pressed() const { return is_action_just_pressed(InputAction::MouseLeft); }
float InputHandler::get_mouse_wheel_move() const { return wheel_move_; }

} // namespace ns