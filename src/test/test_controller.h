#pragma once

#include <string>
#include <optional>

namespace ns {

class Game;

// Simple test controller that reads commands from a pipe
class TestController {
public:
    static TestController& instance();
    
    // Initialize test mode - creates FIFO and starts listening
    void init();
    
    // Process any pending commands (call each frame in test mode)
    void process_commands(Game& game);
    
    // Called after EndDrawing to take screenshot if pending
    void process_screenshot_after_render();
    
    bool is_active() const { return active_; }
    bool has_pending_screenshot() const { return !pending_screenshot_name_.empty(); }
    const std::string& pending_screenshot_name() const { return pending_screenshot_name_; }
    void clear_pending_screenshot() { pending_screenshot_name_.clear(); }

private:
    TestController() = default;
    
    bool active_ = false;
    int pipe_fd_ = -1;
    FILE* pipe_file_ = nullptr;
    std::string pending_screenshot_name_;
    
    // Parse and execute a command
    void execute_command(const std::string& cmd, Game& game);
    
    // Command handlers
    void handle_click(int x, int y, Game& game);
    void handle_key(const std::string& key, Game& game);
    void handle_place_bridge(int gx, int gy, Game& game);
    void handle_start_game(Game& game);
    void handle_quit(Game& game);
    void handle_screenshot(const std::string& name);
    void handle_pan(int dx, int dy, Game& game);
    void handle_zoom(float factor, Game& game);
    
    // Actual screenshot implementation
    void take_screenshot(const std::string& name);
};

} // namespace ns