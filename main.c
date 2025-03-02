#include <stdio.h>

#include "include/window.h"
#include "include/gfx.h"

#define WIDTH 800
#define HEIGHT 600

int main()
{
    window_class_create();

    window_t* window = window_create(WIDTH, HEIGHT, "My Window");
    gfx_t* gfx = gfx_create(window);

    while(update_window_messages())
    {
        gfx_clear(gfx, 0.0f, 0.0f, 0.0f);   

        gfx_draw_test_triangle(gfx);

        gfx_swap_buffers(gfx);
    }

    gfx_destroy(gfx);
    window_destroy(window);
    return 0;
}