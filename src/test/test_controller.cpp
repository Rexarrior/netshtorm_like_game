#include "test_controller.h"
#include "../game.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

// Define CONTROL_PIPE_PATH similar to Python side
#define CONTROL_PIPE_PATH ".game_control_pipe"

namespace ns {

TestController& TestController::instance() {
    static TestController inst;
    return inst;
}

void TestController::init() {
    // Check if test mode is enabled via environment variable
    const char* test_mode = getenv("GAME_TEST_MODE");
    if (!test_mode) {
        active_ = false;
        return;
    }
    
    // Remove existing pipe
    unlink(CONTROL_PIPE_PATH);
    
    // Create FIFO
    if (mkfifo(CONTROL_PIPE_PATH, 0666) != 0) {
        std::cerr << "[TestController] Failed to create pipe: " << CONTROL_PIPE_PATH << std::endl;
        active_ = false;
        return;
    }
    
    // Open pipe for reading (non-blocking)
    pipe_fd_ = open(CONTROL_PIPE_PATH, O_RDONLY | O_NONBLOCK);
    if (pipe_fd_ < 0) {
        std::cerr << "[TestController] Failed to open pipe" << std::endl;
        active_ = false;
        return;
    }
    
    // Set pipe to non-blocking mode
    int flags = fcntl(pipe_fd_, F_GETFL, 0);
    fcntl(pipe_fd_, F_SETFL, flags | O_NONBLOCK);
    
    pipe_file_ = fdopen(pipe_fd_, "r");
    if (!pipe_file_) {
        std::cerr << "[TestController] Failed to fdopen pipe" << std::endl;
        close(pipe_fd_);
        active_ = false;
        return;
    }
    
    active_ = true;
    std::cerr << "[TestController] Test mode active, listening on " << CONTROL_PIPE_PATH << std::endl;
}

void TestController::process_commands(Game& game) {
    if (!active_ || !pipe_file_) return;
    
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe_file_) != nullptr) {
        std::string cmd(buffer);
        // Remove trailing newline
        while (!cmd.empty() && (cmd.back() == '\n' || cmd.back() == '\r')) {
            cmd.pop_back();
        }
        if (!cmd.empty()) {
            std::cerr << "[TestController] Command: " << cmd << std::endl;
            execute_command(cmd, game);
        }
    }
}

void TestController::execute_command(const std::string& cmd, Game& game) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;
    
    if (action == "click") {
        int x, y;
        if (iss >> x >> y) {
            handle_click(x, y, game);
        }
    } else if (action == "key") {
        std::string key;
        if (iss >> key) {
            handle_key(key, game);
        }
    } else if (action == "place_bridge") {
        int gx, gy;
        if (iss >> gx >> gy) {
            handle_place_bridge(gx, gy, game);
        }
    } else if (action == "start_game") {
        handle_start_game(game);
    } else if (action == "quit") {
        handle_quit(game);
    } else if (action == "screenshot") {
        std::string name;
        if (iss >> name) {
            take_screenshot(name);
        }
    } else {
        std::cerr << "[TestController] Unknown command: " << action << std::endl;
    }
}

void TestController::handle_click(int x, int y, Game& game) {
    // Simulate mouse click at screen coordinates
    // For now just trigger a bridge placement at grid position
    // The actual click simulation would require more game integration
    std::cerr << "[TestController] Click at (" << x << ", " << y << ")" << std::endl;
}

void TestController::handle_key(const std::string& key, Game& game) {
    std::cerr << "[TestController] Key: " << key << std::endl;
}

void TestController::handle_place_bridge(int gx, int gy, Game& game) {
    std::cerr << "[TestController] Place bridge at (" << gx << ", " << gy << ")" << std::endl;
}

void TestController::handle_start_game(Game& game) {
    std::cerr << "[TestController] Start game" << std::endl;
}

void TestController::handle_quit(Game& game) {
    std::cerr << "[TestController] Quit requested" << std::endl;
    // Signal the game to close
    // This would need proper integration
}

void TestController::take_screenshot(const std::string& name) {
    std::cerr << "[TestController] Screenshot: " << name << std::endl;
    // Screenshot capture would be handled by external script
    // This just logs the request
}

} // namespace ns