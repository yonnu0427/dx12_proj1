#pragma once

#include <Windows.h>

extern const unsigned int window_width;
extern const unsigned int window_height;

LRESULT WindowProcedure(
    HWND hwnd,
    UINT msg,
    WPARAM wparam,
    LPARAM lparam
);

void CreateGameWindow(
    HWND& hwnd,
    WNDCLASSEX& windowClass
);