cmake_minimum_required(VERSION 3.16)
project(netstorm_like LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib QUIET)
find_package(glfw3 QUIET)
option(BUILD_GAME "Build the main game executable (requires raylib)" ${raylib_FOUND})
option(ENABLE_SCREENSHOT_CAPTURE "Enable offscreen screenshot capture (uses RenderTexture, may impact FPS)" ON)

if(BUILD_GAME AND NOT raylib_FOUND)
    message(FATAL_ERROR "raylib not found. Install it via vcpkg or set BUILD_GAME=OFF to build tests only.")
endif()

if(BUILD_GAME)
    set(SOURCES
        src/main.cpp
        src/game.cpp
        src/core/random.cpp
        src/core/timer.cpp
        src/core/state_machine.cpp
        src/assets/asset_manager.cpp
        src/assets/sprite_sheet.cpp
        src/assets/sound_bank.cpp
        src/input/input_handler.cpp
        src/camera/game_camera.cpp
        src/camera/isometric_camera.cpp
        src/camera/minimap_camera.cpp
        src/map/island.cpp
        src/map/island_generator.cpp
        src/map/bridge.cpp
        src/map/bridge_placer.cpp
        src/map/geyser.cpp
        src/map/map.cpp
        src/map/map_renderer.cpp
        src/ui/menu.cpp
        src/ui/hud.cpp
        src/ui/minimap.cpp
        src/systems/particle_system.cpp
        src/test/test_controller.cpp
    )

    add_executable(netstorm_like ${SOURCES})
    target_include_directories(netstorm_like PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(netstorm_like PRIVATE raylib glfw)
    
    if(ENABLE_SCREENSHOT_CAPTURE)
        target_compile_definitions(netstorm_like PRIVATE ENABLE_SCREENSHOT_CAPTURE)
    endif()

    if(MSVC)
        target_compile_options(netstorm_like PRIVATE /W4 /WX-)
        target_compile_definitions(netstorm_like PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
    else()
        target_compile_options(netstorm_like PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

# ── Tests ────────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(TEST_SOURCES
    tests/test_types.cpp
    tests/test_map.cpp
    tests/test_camera.cpp
    src/camera/isometric_camera.cpp
    src/core/random.cpp
    src/map/island.cpp
    src/map/island_generator.cpp
    src/map/bridge.cpp
    src/map/bridge_placer.cpp
    src/map/map.cpp
)

add_executable(netstorm_tests ${TEST_SOURCES})
target_include_directories(netstorm_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests ${CMAKE_SOURCE_DIR}/src)
target_compile_definitions(netstorm_tests PRIVATE USE_RAYLIB_STUB)
target_link_libraries(netstorm_tests PRIVATE gtest gtest_main)

if(MSVC)
    target_compile_options(netstorm_tests PRIVATE /W4 /WX-)
else()
    target_compile_options(netstorm_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(GoogleTest)
gtest_discover_tests(netstorm_tests)
