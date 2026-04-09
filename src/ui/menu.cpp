#include "menu.h"
#include "../core/config.h"
#include "../assets/asset_manager.h"

namespace ns {

Menu::Menu() = default;

void Menu::render_main_menu() {
    auto& am = AssetManager::instance();
    Texture2D bg = am.get_texture("bg_menu");
    if (bg.id) {
        Rectangle src = {0, 0, static_cast<float>(bg.width), static_cast<float>(bg.height)};
        Rectangle dst = {0, 0, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)};
        DrawTexturePro(bg, src, dst, {0, 0}, 0, WHITE);
    } else {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, {26, 26, 46, 255});
    }

    draw_centered_text("NETSTORM-LIKE", 150, 48, GOLD);
    draw_centered_text("Islands at War", 210, 24, SKYBLUE);

    std::vector<MenuItem> items;
    items.push_back({"Start Game", on_start_});
    items.push_back({"Quit", on_quit_});
    draw_menu_buttons(items, 320);
}

void Menu::render_paused() {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));
    draw_centered_text("PAUSED", SCREEN_HEIGHT / 2 - 60, 36, WHITE);

    std::vector<MenuItem> items;
    items.push_back({"Resume", on_resume_});
    items.push_back({"Quit to Menu", on_quit_});
    draw_menu_buttons(items, SCREEN_HEIGHT / 2);
}

void Menu::render_game_over(bool victory, int floor, int level) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));
    if (victory) {
        draw_centered_text("VICTORY!", SCREEN_HEIGHT / 2 - 60, 48, GOLD);
    } else {
        draw_centered_text("DEFEAT", SCREEN_HEIGHT / 2 - 60, 48, RED);
    }
    draw_centered_text("Floor: " + std::to_string(floor) + "  Level: " + std::to_string(level),
                       SCREEN_HEIGHT / 2, 20, LIGHTGRAY);
    draw_centered_text("Press ENTER to continue", SCREEN_HEIGHT / 2 + 60, 18, GRAY);
}

void Menu::set_on_start(std::function<void()> cb) { on_start_ = std::move(cb); }
void Menu::set_on_resume(std::function<void()> cb) { on_resume_ = std::move(cb); }
void Menu::set_on_quit(std::function<void()> cb) { on_quit_ = std::move(cb); }

void Menu::draw_centered_text(const std::string& text, int y, int size, Color color) const {
    int text_width = MeasureText(text.c_str(), size);
    DrawText(text.c_str(), (SCREEN_WIDTH - text_width) / 2, y, size, color);
}

void Menu::draw_menu_buttons(const std::vector<MenuItem>& items, int start_y) const {
    int button_width = 200;
    int button_height = 40;
    int spacing = 20;

    for (size_t i = 0; i < items.size(); ++i) {
        int x = (SCREEN_WIDTH - button_width) / 2;
        int y = start_y + static_cast<int>(i) * (button_height + spacing);

        Vector2 mouse = GetMousePosition();
        bool hovered = mouse.x >= x && mouse.x <= x + button_width &&
                       mouse.y >= y && mouse.y <= y + button_height;

        Color bg = hovered ? DARKGRAY : GRAY;
        DrawRectangle(x, y, button_width, button_height, bg);
        DrawRectangleLines(x, y, button_width, button_height, WHITE);

        int text_width = MeasureText(items[i].text.c_str(), 20);
        DrawText(items[i].text.c_str(), x + (button_width - text_width) / 2, y + 10, 20, WHITE);

        if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (items[i].action) items[i].action();
        }
    }
}

} // namespace ns
