#pragma once

#ifdef GFX_H_INTERNALS
#include <d3d11.h>

#define gfx_foreach_COM_object()\
    X(IDXGISwapChain, swap_chain)\
    X(ID3D11Device, device)\
    X(ID3D11DeviceContext, context)\
    X(ID3D11RenderTargetView, target)

struct gfx_t
{
    #define X(TYPE, NAME) TYPE* NAME;
    gfx_foreach_COM_object()
    #undef X
};
#endif

typedef struct gfx_t gfx_t;

gfx_t* gfx_create(window_t* window);
void gfx_clear(const gfx_t* gfx, float red, float green, float blue);
void gfx_draw_test_triangle(const gfx_t* gfx);
void gfx_swap_buffers(const gfx_t* gfx);
void gfx_destroy(gfx_t* gfx);
