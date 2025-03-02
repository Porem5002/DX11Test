#pragma once

#ifdef GFX_H_INTERNALS
#include <d3d11.h>

struct gfx_t
{
    IDXGISwapChain* swap_chain;
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    ID3D11RenderTargetView* target;
};
#endif

typedef struct gfx_t gfx_t;

gfx_t* gfx_create(window_t* window);
void gfx_clear(const gfx_t* gfx, float red, float green, float blue);
void gfx_draw_test_triangle(const gfx_t* gfx);
void gfx_swap_buffers(const gfx_t* gfx);
void gfx_destroy(gfx_t* gfx);
