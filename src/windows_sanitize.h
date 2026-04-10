#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef CloseWindow
#undef ShowCursor
#undef Rectangle
#undef PlaySound
#undef DrawText
#undef LoadImage
#undef GetMessage
#endif