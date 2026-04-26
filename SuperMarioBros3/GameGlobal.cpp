#include "GameGlobal.h"

// Khởi tạo giá trị ban đầu (NULL hoặc 0) cho tất cả các biến static
HINSTANCE GameGlobal::hInstance = NULL;
HWND GameGlobal::hWnd = NULL;
int GameGlobal::backBufferWidth = 500;
int GameGlobal::backBufferHeight = 600;

ID3D10Device* GameGlobal::pD3DDevice = NULL;
IDXGISwapChain* GameGlobal::pSwapChain = NULL;
ID3D10RenderTargetView* GameGlobal::pRenderTargetView = NULL;

ID3DX10Sprite* GameGlobal::spriteObject = NULL;
ID3D10BlendState* GameGlobal::pBlendStateAlpha = NULL;
ID3D10SamplerState* GameGlobal::pPointSamplerState = NULL; 
