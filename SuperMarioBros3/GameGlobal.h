#pragma once

#include <Windows.h>
#include <d3d10.h>
#include <d3dx10.h>

class GameGlobal
{
public:
    // 1. Thông số Cửa sổ (Windows)
    static HINSTANCE hInstance;
    static HWND hWnd;
    static int backBufferWidth;
    static int backBufferHeight;

    // 2. Bộ máy DirectX 10
    static ID3D10Device* pD3DDevice;
    static IDXGISwapChain* pSwapChain;
    static ID3D10RenderTargetView* pRenderTargetView;

    // 3. Trình quản lý vẽ ảnh
    static ID3DX10Sprite* spriteObject;
    static ID3D10BlendState* pBlendStateAlpha;
    static ID3D10SamplerState* pPointSamplerState;

    // Hàm tiện ích lấy kích thước màn hình
    static int GetWidth() { return backBufferWidth; }
    static int GetHeight() { return backBufferHeight; }
};