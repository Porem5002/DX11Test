#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <d3dcompiler.h>

#include "include/comhelpers.h"

#define WINDOW_H_INTERNALS
#include "include/window.h"

#define GFX_H_INTERNALS
#include "include/gfx.h"

gfx_t* gfx_create(window_t* window)
{
    gfx_t* gfx = malloc(sizeof(gfx_t));
    assert(gfx != NULL);

    DXGI_SWAP_CHAIN_DESC swap_chain_desc = { 0 };
    swap_chain_desc.BufferDesc.Width = 0;
    swap_chain_desc.BufferDesc.Height = 0;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 0;
    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = 1;
    swap_chain_desc.OutputWindow = window->hWnd;
    swap_chain_desc.Windowed = TRUE;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swap_chain_desc.Flags = 0;

    IDXGISwapChain* swap_chain;
    ID3D11Device* device;
    ID3D11DeviceContext* context;

    HRESULT res = D3D11CreateDeviceAndSwapChain(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        D3D11_CREATE_DEVICE_DEBUG,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &swap_chain_desc, &swap_chain, &device, NULL, &context
    );

    if(FAILED(res))
    {
        printf("Error creating device and swap chain: %d(%x)\n", res, res);
        abort();
    }

    ID3D11Resource* back_buffer = NULL;
    COMCALL(swap_chain, GetBuffer, 0, &IID_ID3D11Resource, (void**)&back_buffer);
    
    ID3D11RenderTargetView* target = NULL;
    COMCALL(device, CreateRenderTargetView, back_buffer, NULL, &target);

    COMCALL0(back_buffer, Release);

    gfx->swap_chain = swap_chain;
    gfx->device = device;
    gfx->context = context;
    gfx->target = target;

    return gfx;
}

void gfx_clear(const gfx_t* gfx, float red, float green, float blue)
{
    float color[] = { red, green, blue, 1.0f };
    COMCALL(gfx->context, ClearRenderTargetView, gfx->target, color);
}

void gfx_draw_test_triangle(const gfx_t* gfx)
{
    HRESULT hr;

    typedef struct
    {
        float x;
        float y;
    } vertex_t;

    const vertex_t vertices [] = {
        { 0.0f, 0.5f },
        { 0.5f, -0.5f },
        { -0.5f, -0.5f },
    };

    D3D11_BUFFER_DESC bd = {0};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(vertex_t);

    D3D11_SUBRESOURCE_DATA srd = {0};
    srd.pSysMem = vertices; 

    ID3D11Buffer* vertex_buffer = NULL;
    hr = COMCALL(gfx->device, CreateBuffer, &bd, &srd, &vertex_buffer);
    if(FAILED(hr))
    {
        char buf [1024];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, buf, sizeof(buf), NULL);
        printf("Error calling gfx->device->CreateBuffer: %s\n", buf);
        printf("Location: %s:%d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    const UINT stride = sizeof(vertex_t);
    const UINT offset = 0;
    COMCALL(gfx->context, IASetVertexBuffers, 0, 1, &vertex_buffer, &stride, &offset);

    COMCALL(gfx->context, IASetPrimitiveTopology, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3DBlob* vertex_blob = NULL;
    ID3DBlob* vertex_error_blob = NULL;
    hr = D3DCompileFromFile(L"shaders\\vs.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &vertex_blob, &vertex_error_blob);
    if(FAILED(hr))
    {
        char buf [1024];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, buf, sizeof(buf), NULL);
        printf("Error calling D3DCompileFromFile: %s\n", buf);

        if(vertex_error_blob != NULL)
            printf("Shader Compilation Errors:\n  %s\n", (char*)COMCALL0(vertex_error_blob, GetBufferPointer));

        printf("Location: %s:%d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    assert(vertex_error_blob == NULL);

    const D3D11_INPUT_ELEMENT_DESC ied [] = {
        { "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    ID3D11InputLayout* layout = NULL;
    hr = COMCALL(gfx->device, CreateInputLayout,
        ied,
        1,
        COMCALL0(vertex_blob, GetBufferPointer),
        COMCALL0(vertex_blob, GetBufferSize),
        &layout
    );

    if(FAILED(hr))
    {
        char buf [1024];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, buf, sizeof(buf), NULL);
        printf("Error calling gfx->device->CreateInputLayout: %s\n", buf);
        printf("Location: %s:%d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    COMCALL(gfx->context, IASetInputLayout, layout);

    ID3D11VertexShader* vertex_shader = NULL;
    hr = COMCALL(gfx->device, CreateVertexShader,
        COMCALL0(vertex_blob, GetBufferPointer),
        COMCALL0(vertex_blob, GetBufferSize),
        NULL,
        &vertex_shader
    );

    if(FAILED(hr))
    {
        char buf [1024];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, buf, sizeof(buf), NULL);
        printf("Error calling gfx->device->CreateVertexShader: %s\n", buf);
        printf("Location: %s:%d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    COMCALL(gfx->context, VSSetShader, vertex_shader, NULL, 0);

    ID3DBlob* pixel_blob = NULL;
    ID3DBlob* pixel_error_blob = NULL;
    hr = D3DCompileFromFile(L"shaders\\ps.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &pixel_blob, &pixel_error_blob);
    if(FAILED(hr))
    {
        char buf [1024];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, buf, sizeof(buf), NULL);
        printf("Error calling D3DCompileFromFile: %s\n", buf);

        if(pixel_error_blob != NULL)
            printf("Shader Compilation Errors:\n  %s\n", (char*)COMCALL0(pixel_error_blob, GetBufferPointer));

        printf("Location: %s:%d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    ID3D11PixelShader* pixel_shader = NULL;
    hr = COMCALL(gfx->device, CreatePixelShader,
        COMCALL0(pixel_blob, GetBufferPointer),
        COMCALL0(pixel_blob, GetBufferSize),
        NULL,
        &pixel_shader
    );

    if(FAILED(hr))
    {
        char buf [1024];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, buf, sizeof(buf), NULL);
        printf("Error calling gfx->device->CreatePixelShader: %s\n", buf);
        printf("Location: %s:%d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    assert(pixel_error_blob == NULL);

    COMCALL(gfx->context, PSSetShader, pixel_shader, NULL, 0);

    COMCALL(gfx->context, OMSetRenderTargets, 1, &gfx->target, NULL);

    D3D11_VIEWPORT viewport = {0};
    viewport.Width = 800;
    viewport.Height = 600;
    viewport.MaxDepth = 1;
    viewport.MinDepth = 0;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    COMCALL(gfx->context, RSSetViewports, 1, &viewport);

    COMCALL(gfx->context, Draw, 3, 0);

    COMCALL0(vertex_buffer, Release);
    COMCALL0(vertex_blob, Release);
    COMCALL0(layout, Release);
    
    COMCALL0(vertex_shader, Release);

    COMCALL0(pixel_blob, Release);
    COMCALL0(pixel_shader, Release);
}

void gfx_swap_buffers(const gfx_t* gfx)
{
    HRESULT hr = COMCALL(gfx->swap_chain, Present, 1, 0);
}

void gfx_destroy(gfx_t* gfx)
{
    if(gfx->swap_chain != NULL)
        COMCALL0(gfx->swap_chain, Release);

    if(gfx->device != NULL)
        COMCALL0(gfx->device, Release);

    if(gfx->context != NULL)
        COMCALL0(gfx->context, Release);

    if(gfx->target != NULL)
        COMCALL0(gfx->target, Release);

    free(gfx);
}