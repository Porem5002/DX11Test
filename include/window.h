#pragma once

#ifdef WINDOW_H_INTERNALS
#include <windows.h>

struct window_t
{
    HWND hWnd;
};
#endif

#include <stdbool.h>

typedef struct window_t window_t;

void window_class_create();
window_t* window_create(int width, int height, const char* title);
bool update_window_messages();
void window_destroy(window_t* window);