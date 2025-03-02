#include <assert.h>
#include <stdio.h>

#define WINDOW_H_INTERNALS
#include "include/window.h"

#define WND_CLASS_NAME "wclass"

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SETCURSOR:
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

void window_class_create()
{
    WNDCLASSEXA wclass = { 0 };
    wclass.cbSize = sizeof(wclass);
    wclass.hInstance = GetModuleHandleA(NULL);
    wclass.style = CS_OWNDC;
    wclass.lpszClassName = WND_CLASS_NAME;
    wclass.lpfnWndProc = WndProc;

    assert(RegisterClassExA(&wclass) != 0);
}

window_t* window_create(int width, int height, const char* title)
{
    HINSTANCE hInstance = GetModuleHandleA(NULL);

    window_t* window = malloc(sizeof(window_t));
    assert(window != NULL);

    HWND hWnd = CreateWindowA(
        WND_CLASS_NAME, title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, hInstance, NULL
    );

    assert(hWnd != NULL);
    
    ShowWindow(hWnd, SW_SHOW);

    window->hWnd = hWnd;
    return window;
}

bool update_window_messages()
{
    MSG msg;

    while(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if(msg.message == WM_QUIT) return false;

        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return true;
}

void window_destroy(window_t* window)
{
    if(IsWindow(window->hWnd))
        DestroyWindow(window->hWnd);
    
    free(window);
}