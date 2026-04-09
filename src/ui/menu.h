#pragma once

#include <raylib.h>
#include "../core/types.h"
#include <string>
#include <vector>
#include <functional>

namespace ns {

struct MenuItem {
    std::string text;
    std::function<void()> action;
};

class Menu {
public:
    Menu();
    void render_main_menu();
    void render_paused();
    void render_game_over(bool victory, int floor, int level);
    void set_on_start(std::function<void()> cb);
    void set_on_resume(std::function<void()> cb);
    void set_on_quit(std::function<void()> cb);

private:
    void draw_centered_text(const std::string& text, int y, int size, Color color) const;
    void draw_menu_buttons(const std::vector<MenuItem>& items, int start_y) const;

    std::function<void()> on_start_;
    std::function<void()> on_resume_;
    std::function<void()> on_quit_;
};

} // namespace ns
