#include "test_controller.h"
#include "../game.h"
#include <raylib.h>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <ctime>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif

#define SCREENSHOTS_PATH "test_screenshots"

namespace ns {

TestController& TestController::instance() {
    static TestController inst;
    return inst;
}

void TestController::init() {
    const char* test_mode = getenv("GAME_TEST_MODE");
    if (!test_mode) {
        active_ = false;
        return;
    }
    
#ifdef _WIN32
    active_ = false;
    std::cerr << "[TestController] Test mode disabled on Windows (FIFO not supported)" << std::endl;
#else
    unlink(CONTROL_PIPE_PATH);
    
    if (mkfifo(CONTROL_PIPE_PATH, 0666) != 0) {
        std::cerr << "[TestController] Failed to create pipe: " << CONTROL_PIPE_PATH << std::endl;
        active_ = false;
        return;
    }
    
    std::cerr << "[TestController] Opening pipe..." << std::endl;
    
    pipe_fd_ = open(CONTROL_PIPE_PATH, O_RDONLY);
    if (pipe_fd_ < 0) {
        std::cerr << "[TestController] Failed to open pipe: " << strerror(errno) << std::endl;
        active_ = false;
        return;
    }
    
    pipe_file_ = fdopen(pipe_fd_, "r");
    if (!pipe_file_) {
        std::cerr << "[TestController] Failed to fdopen pipe" << std::endl;
        close(pipe_fd_);
        active_ = false;
        return;
    }
    
    setlinebuf(pipe_file_);
    
    active_ = true;
    std::cerr << "[TestController] Test mode active, listening on " << CONTROL_PIPE_PATH << std::endl;
#endif
}

void TestController::process_commands(Game& game) {
    if (!active_) return;
    
#ifndef _WIN32
    if (!pipe_file_) return;
    
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(pipe_fd_, &read_fds);
    
    struct timeval tv = {0, 0};
    
    if (select(pipe_fd_ + 1, &read_fds, nullptr, nullptr, &tv) > 0) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe_file_) != nullptr) {
            std::string cmd(buffer);
            while (!cmd.empty() && (cmd.back() == '\n' || cmd.back() == '\r')) {
                cmd.pop_back();
            }
            if (!cmd.empty()) {
                std::cerr << "[TestController] Command: " << cmd << std::endl;
                execute_command(cmd, game);
            }
        }
    }
#endif
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
            handle_screenshot(name);
        }
    } else if (action == "pan") {
        int dx, dy;
        if (iss >> dx >> dy) {
            handle_pan(dx, dy, game);
        }
    } else if (action == "zoom") {
        float factor;
        if (iss >> factor) {
            handle_zoom(factor, game);
        }
    } else {
        std::cerr << "[TestController] Unknown command: " << action << std::endl;
    }
}

void TestController::handle_click(int x, int y, Game& game) {
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
    game.start_game();
}

void TestController::handle_quit(Game& game) {
    std::cerr << "[TestController] Quit requested" << std::endl;
}

void TestController::handle_screenshot(const std::string& name) {
    std::cerr << "[TestController] Screenshot requested: " << name << std::endl;
    pending_screenshot_name_ = name;
}

void TestController::handle_pan(int dx, int dy, Game& game) {
    std::cerr << "[TestController] Pan camera by (" << dx << ", " << dy << ")" << std::endl;
    game.pan_camera(dx, dy);
}

void TestController::handle_zoom(float factor, Game& game) {
    std::cerr << "[TestController] Zoom camera by factor " << factor << std::endl;
    game.zoom_camera(factor);
}

void TestController::process_screenshot_after_render() {
    if (!pending_screenshot_name_.empty()) {
        std::cerr << "[TestController] Processing pending screenshot: " << pending_screenshot_name_ << std::endl;
        take_screenshot(pending_screenshot_name_);
        pending_screenshot_name_.clear();
    }
}

void TestController::take_screenshot(const std::string& name) {
#ifdef _WIN32
    if (_mkdir(SCREENSHOTS_PATH) != 0 && errno != 0 && errno != EEXIST) {
        return;
    }
#else
    struct stat st;
    if (stat(SCREENSHOTS_PATH, &st) != 0) {
        if (mkdir(SCREENSHOTS_PATH, 0755) != 0 && errno != EEXIST) {
            return;
        }
    }
#endif
    
    std::string final_path = std::string(SCREENSHOTS_PATH) + "/" + name + ".png";
    
    std::cerr << "[TestController] Screenshot: " << final_path << std::endl;
    
    Image screen = LoadImageFromScreen();
    ExportImage(screen, final_path.c_str());
    UnloadImage(screen);
    
    std::cerr << "[TestController] Saved: " << final_path << std::endl;
}

} // namespace ns