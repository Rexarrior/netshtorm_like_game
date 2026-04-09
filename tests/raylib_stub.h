#pragma once

// Raylib stub for unit testing — replaces raylib.h when USE_RAYLIB_STUB is defined.
// All functions are inline no-ops, all types are minimal stubs.

#ifdef USE_RAYLIB_STUB

#include <string>

namespace ns {

struct Vector2 { float x, y; };
struct Vector2i { int x, y; };
struct Color { unsigned char r, g, b, a; };
struct Texture2D { unsigned int id; int width, height; int mipmaps; int format; };
struct Camera2D { Vector2 offset, target; float rotation, zoom; };
struct Rectangle { float x, y, width, height; };
struct Sound { unsigned int id; };
struct Music { void* ctxData; unsigned int streamId; };

inline int MeasureText(const char*, int) { return 0; }
inline void DrawText(const char*, int, int, int, Color) {}
inline void DrawRectangle(int, int, int, int, Color) {}
inline void DrawRectangleLines(int, int, int, int, Color) {}
inline void DrawTriangle(Vector2, Vector2, Vector2, Color) {}
inline void DrawCircle(int, int, int, Color) {}
inline void DrawCircleV(Vector2, float, Color) {}
inline void DrawPixel(int, int, Color) {}
inline Color Fade(Color, float) { return Color{}; }
inline Color ColorTint(Color, float) { return Color{}; }
inline Vector2 GetWorldToScreen2D(Vector2, Camera2D) { return Vector2{}; }
inline Vector2 GetScreenToWorld2D(Vector2, Camera2D) { return Vector2{}; }
inline int GetFPS() { return 60; }
inline float GetFrameTime() { return 0.016f; }
inline bool IsKeyDown(int) { return false; }
inline bool IsKeyPressed(int) { return false; }
inline bool IsMouseButtonDown(int) { return false; }
inline bool IsMouseButtonPressed(int) { return false; }
inline Vector2 GetMousePosition() { return Vector2{}; }
inline float GetMouseWheelMove() { return 0; }
inline void InitWindow(int, int, const char*) {}
inline void CloseWindow() {}
inline void SetTargetFPS(int) {}
inline bool WindowShouldClose() { return false; }
inline void BeginDrawing() {}
inline void EndDrawing() {}
inline void BeginMode2D(Camera2D) {}
inline void EndMode2D() {}
inline void ClearBackground(Color) {}
inline Texture2D LoadTexture(const char*) { return Texture2D{0,0,0,0,0}; }
inline void UnloadTexture(Texture2D) {}
inline Sound LoadSound(const char*) { return Sound{0}; }
inline void UnloadSound(Sound) {}
inline void PlaySound(Sound) {}
inline Music LoadMusicStream(const char*) { return Music{}; }
inline void UnloadMusicStream(Music) {}

constexpr Color BLACK{0,0,0,255};
constexpr Color WHITE{255,255,255,255};
constexpr Color GRAY{128,128,128,255};
constexpr Color DARKGRAY{64,64,64,255};
constexpr Color LIGHTGRAY{192,192,192,255};
constexpr Color GOLD{255,215,0,255};
constexpr Color SKYBLUE{135,206,235,255};
constexpr Color RED{255,0,0,255};
constexpr Color GREEN{0,255,0,255};
constexpr Color BLUE{0,0,255,255};
constexpr Color YELLOW{255,255,0,255};
constexpr Color MAGENTA{255,0,255,255};

constexpr int KEY_W=0, KEY_S=0, KEY_A=0, KEY_D=0;
constexpr int KEY_UP=0, KEY_DOWN=0, KEY_LEFT=0, KEY_RIGHT=0;
constexpr int KEY_SPACE=0, KEY_ESCAPE=0, KEY_ENTER=0;
constexpr int KEY_EQUAL=0, KEY_MINUS=0;
constexpr int KEY_KP_ADD=0, KEY_KP_SUBTRACT=0;
constexpr int MOUSE_BUTTON_LEFT=0, MOUSE_BUTTON_RIGHT=0, MOUSE_BUTTON_MIDDLE=0;

} // namespace ns

#endif // USE_RAYLIB_STUB
