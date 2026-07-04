#include <fstream>

#include "Game.h"
#include "debug.h"
#include "Utils.h"

#include "Texture.h"
#include "Animations.h"
#include "PlayScene.h"
#include "WorldScene.h"
#include "GameGlobal.h"
#include "SceneManager.h"
#include "SoundManager.h"

/*
	Initialize DirectX, create a Direct3D device for rendering within the window, initial Sprite library for
	rendering 2D images
	- hWnd: Application window handle
*/
void Game::Init(HWND hWnd, HINSTANCE hInstance)
{
	GameGlobal::hWnd = hWnd;
	GameGlobal::hInstance = hInstance;

	// retrieve client area width & height so that we can create backbuffer height & width accordingly 
	RECT r;
	GetClientRect(hWnd, &r);

	GameGlobal::backBufferWidth = r.right + 1;
	GameGlobal::backBufferHeight = r.bottom + 1;

	DebugOut(L"[INFO] Window's client area: width= %d, height= %d\n", r.right - 1, r.bottom - 1);

	// Create & clear the DXGI_SWAP_CHAIN_DESC structure
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Fill in the needed values
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = GameGlobal::backBufferWidth;
	swapChainDesc.BufferDesc.Height = GameGlobal::backBufferHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;

	// Create the D3D device and the swap chain
	HRESULT hr = D3D10CreateDeviceAndSwapChain(NULL,
		D3D10_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		D3D10_SDK_VERSION,
		&swapChainDesc,
		&GameGlobal::pSwapChain,
		&GameGlobal::pD3DDevice);

	if (hr != S_OK)
	{
		DebugOut((wchar_t*)L"[ERROR] D3D10CreateDeviceAndSwapChain has failed %s %d", _W(__FILE__), __LINE__);
		return;
	}

	// Get the back buffer from the swapchain
	ID3D10Texture2D* pBackBuffer;
	hr = GameGlobal::pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	if (hr != S_OK)
	{
		DebugOut((wchar_t*)L"[ERROR] pSwapChain->GetBuffer has failed %s %d", _W(__FILE__), __LINE__);
		return;
	}

	// create the render target view
	hr = GameGlobal::pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &GameGlobal::pRenderTargetView);

	pBackBuffer->Release();
	if (hr != S_OK)
	{
		DebugOut((wchar_t*)L"[ERROR] CreateRenderTargetView has failed %s %d", _W(__FILE__), __LINE__);
		return;
	}

	// set the render target
	GameGlobal::pD3DDevice->OMSetRenderTargets(1, &GameGlobal::pRenderTargetView, NULL);

	// create and set the viewport
	D3D10_VIEWPORT viewPort;
	viewPort.Width = GameGlobal::backBufferWidth;
	viewPort.Height = GameGlobal::backBufferHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	GameGlobal::pD3DDevice->RSSetViewports(1, &viewPort);

	//
	//
	//

	D3D10_SAMPLER_DESC desc; 
	desc.Filter = D3D10_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	desc.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
	desc.MipLODBias = 0;
	desc.MaxAnisotropy = 1;
	desc.ComparisonFunc = D3D10_COMPARISON_NEVER;
	desc.BorderColor[0] = 1.0f;
	desc.BorderColor[1] = 1.0f;
	desc.BorderColor[2] = 1.0f;
	desc.BorderColor[3] = 1.0f;
	desc.MinLOD = -FLT_MAX;
	desc.MaxLOD = FLT_MAX;

	GameGlobal::pD3DDevice->CreateSamplerState(&desc, &GameGlobal::pPointSamplerState);

	// create the sprite object to handle sprite drawing 
	hr = D3DX10CreateSprite(GameGlobal::pD3DDevice, 0, &GameGlobal::spriteObject);

	if (hr != S_OK)
	{
		DebugOut((wchar_t*)L"[ERROR] D3DX10CreateSprite has failed %s %d", _W(__FILE__), __LINE__);
		return;
	}

	D3DXMATRIX matProjection;

	// Create the projection matrix using the values in the viewport
	D3DXMatrixOrthoOffCenterLH(&matProjection,
		(float)viewPort.TopLeftX,
		(float)viewPort.Width,
		(float)viewPort.TopLeftY,
		(float)viewPort.Height,
		0.1f,
		10);
	hr = GameGlobal::spriteObject->SetProjectionTransform(&matProjection);

	// Initialize the blend state for alpha drawing
	D3D10_BLEND_DESC StateDesc;
	ZeroMemory(&StateDesc, sizeof(D3D10_BLEND_DESC));
	StateDesc.AlphaToCoverageEnable = FALSE;
	StateDesc.BlendEnable[0] = TRUE;
	StateDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	StateDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	StateDesc.BlendOp = D3D10_BLEND_OP_ADD;
	StateDesc.SrcBlendAlpha = D3D10_BLEND_ZERO;
	StateDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	StateDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	StateDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	GameGlobal::pD3DDevice->CreateBlendState(&StateDesc, &GameGlobal::pBlendStateAlpha);

	DebugOut((wchar_t*)L"[INFO] InitDirectX has been successful\n");

	return;
}

void Game::SetPointSamplerState()
{
	GameGlobal::pD3DDevice->VSSetSamplers(0, 1, &GameGlobal::pPointSamplerState);
	GameGlobal::pD3DDevice->GSSetSamplers(0, 1, &GameGlobal::pPointSamplerState);
	GameGlobal::pD3DDevice->PSSetSamplers(0, 1, &GameGlobal::pPointSamplerState);
}

/*
	Draw the whole texture or part of texture onto screen
	NOTE: This function is very inefficient because it has to convert
	from texture to sprite every time we need to draw it
*/
void Game::Draw(float x, float y, LPTEXTURE tex, RECT* rect, float alpha, int sprite_width, int sprite_height)
{
	if (tex == NULL) return;

	int spriteWidth = sprite_width;
	int spriteHeight = sprite_height;

	D3DX10_SPRITE sprite;

	// Set the sprite’s shader resource view
	sprite.pTexture = tex->getShaderResourceView();

	if (rect == NULL)
	{
		// top-left location in U,V coords
		sprite.TexCoord.x = 0;
		sprite.TexCoord.y = 0;

		// Determine the texture size in U,V coords
		sprite.TexSize.x = 1.0f;
		sprite.TexSize.y = 1.0f;

		if (spriteWidth==0) spriteWidth = tex->getWidth();
		if (spriteHeight==0) spriteHeight = tex->getHeight();
	}
	else
	{
		sprite.TexCoord.x = rect->left / (float)tex->getWidth();
		sprite.TexCoord.y = rect->top / (float)tex->getHeight();

		if (spriteWidth == 0) spriteWidth = (rect->right - rect->left + 1);
		if (spriteHeight == 0) spriteHeight = (rect->bottom - rect->top + 1);

		sprite.TexSize.x = spriteWidth / (float)tex->getWidth();
		sprite.TexSize.y = spriteHeight / (float)tex->getHeight();
	}

	// Set the texture index. Single textures will use 0
	sprite.TextureIndex = 0;

	// The color to apply to this sprite, full color applies white.
	//sprite.ColorModulate = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	sprite.ColorModulate = D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha);


	//
	// Build the rendering matrix based on sprite location 
	//

	// The translation matrix to be created
	D3DXMATRIX matTranslation;

	// Create the translation matrix
	D3DXMatrixTranslation(&matTranslation, x, (GameGlobal::backBufferHeight - y), 0.1f);

	// Scale the sprite to its correct width and height because by default, DirectX draws it with width = height = 1.0f 
	D3DXMATRIX matScaling;
	D3DXMatrixScaling(&matScaling, (FLOAT)spriteWidth, (FLOAT)spriteHeight, 1.0f);

	// Setting the sprite’s position and size
	sprite.matWorld = (matScaling * matTranslation);

	GameGlobal::spriteObject->DrawSpritesImmediate(&sprite, 1, 0, 0);
}

/*
	Utility function to wrap D3DXCreateTextureFromFileEx
*/
LPTEXTURE Game::LoadTexture(LPCWSTR texturePath)
{
	ID3D10Resource* pD3D10Resource = NULL;
	ID3D10Texture2D* tex = NULL;

	// Retrieve image information first 
	D3DX10_IMAGE_INFO imageInfo;
	HRESULT hr = D3DX10GetImageInfoFromFile(texturePath, NULL, &imageInfo, NULL);
	if (FAILED(hr))
	{
		DebugOut((wchar_t*)L"[ERROR] D3DX10GetImageInfoFromFile failed for  file: %s with error: %d\n", texturePath, hr);
		return NULL;
	}

	D3DX10_IMAGE_LOAD_INFO info; 
	ZeroMemory(&info, sizeof(D3DX10_IMAGE_LOAD_INFO));
	info.Width = imageInfo.Width;
	info.Height = imageInfo.Height;
	info.Depth = imageInfo.Depth;
	info.FirstMipLevel = 0;
	info.MipLevels = 1;
	info.Usage = D3D10_USAGE_DEFAULT;
	info.BindFlags = D3DX10_DEFAULT;
	info.CpuAccessFlags = D3DX10_DEFAULT;
	info.MiscFlags = D3DX10_DEFAULT;
	info.Format = imageInfo.Format;
	info.Filter = D3DX10_FILTER_NONE;
	info.MipFilter = D3DX10_DEFAULT;
	info.pSrcInfo = &imageInfo;

	// Loads the texture into a temporary ID3D10Resource object
	hr = D3DX10CreateTextureFromFile(GameGlobal::pD3DDevice,
		texturePath,
		&info,
		NULL,
		&pD3D10Resource,
		NULL);

	// Make sure the texture was loaded successfully
	if (FAILED(hr))
	{
		DebugOut((wchar_t*)L"[ERROR] Failed to load texture file: %s with error: %d\n", texturePath, hr);
		return NULL;
	}

	// Translates the ID3D10Resource object into a ID3D10Texture2D object
	pD3D10Resource->QueryInterface(__uuidof(ID3D10Texture2D), (LPVOID*)&tex);
	pD3D10Resource->Release();

	if (!tex) {
		DebugOut((wchar_t*)L"[ERROR] Failed to convert from ID3D10Resource to ID3D10Texture2D \n");
		return NULL;
	}

	//
	// Create the Share Resource View for this texture 
	// 	   
	// Get the texture details
	D3D10_TEXTURE2D_DESC desc;
	tex->GetDesc(&desc);

	// Create a shader resource view of the texture
	D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;

	// Clear out the shader resource view description structure
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));

	// Set the texture format
	SRVDesc.Format = desc.Format;

	// Set the type of resource
	SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = desc.MipLevels;

	ID3D10ShaderResourceView* gSpriteTextureRV = NULL;

	GameGlobal::pD3DDevice->CreateShaderResourceView(tex, &SRVDesc, &gSpriteTextureRV);

	DebugOut(L"[INFO] Texture loaded Ok from file: %s \n", texturePath);

	return new Texture(tex, gSpriteTextureRV);
}


#define MAX_GAME_LINE 1024


#define GAME_FILE_SECTION_UNKNOWN -1
#define GAME_FILE_SECTION_SETTINGS 1
#define GAME_FILE_SECTION_SCENES 2
#define GAME_FILE_SECTION_TEXTURES 3
#define GAME_FILE_SECTION_AUDIO 4
/*
	Load game campaign file and load/initiate first scene
*/
void Game::Load(LPCWSTR gameFile)
{
	DebugOut(L"[INFO] Start loading game file : %s\n", gameFile);
	SoundManager::GetInstance()->Init();

	ifstream f;
	f.open(gameFile);
	char str[MAX_GAME_LINE];

	// current resource section flag
	int section = GAME_FILE_SECTION_UNKNOWN;

	while (f.getline(str, MAX_GAME_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[SETTINGS]") { section = GAME_FILE_SECTION_SETTINGS; continue; }
		if (line == "[TEXTURES]") { section = GAME_FILE_SECTION_TEXTURES; continue; }
		if (line == "[SCENES]") { section = GAME_FILE_SECTION_SCENES; continue; }
		if (line == "[AUDIO]") { section = GAME_FILE_SECTION_AUDIO; continue; }
		if (line[0] == '[')
		{
			section = GAME_FILE_SECTION_UNKNOWN;
			DebugOut(L"[ERROR] Unknown section: %s\n", ToLPCWSTR(line));
			continue;
		}

		//
		// data section
		//
		switch (section)
		{
		case GAME_FILE_SECTION_SETTINGS: _ParseSection_SETTINGS(line); break;
		case GAME_FILE_SECTION_SCENES: _ParseSection_SCENES(line); break;
		case GAME_FILE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case GAME_FILE_SECTION_AUDIO: _ParseSection_AUDIO(line); break;
		}
	}
	f.close();

	DebugOut(L"[INFO] Loading game file : %s has been loaded successfully\n", gameFile);

	SceneManager::GetInstance()->SwitchScene();
}


void Game::_ParseSection_SETTINGS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2)
	{
		return;
	}
	if (tokens[0] == "start")
	{
		int scene_id = atoi(tokens[1].c_str());
		SceneManager::GetInstance()->InitiateSwitchScene(scene_id);
	}
	if (tokens[0] == "DevModeBypassMap")
	{
		GameManager::GetInstance()->isDevMode_BypassMapLock = atoi(tokens[1].c_str());
	}
	else
	{
		DebugOut(L"[ERROR] Unknown game setting: %s\n", ToWSTR(tokens[0]).c_str());
	}
}

void Game::_ParseSection_SCENES(string line)
{
	vector<string> tokens = split(line);
	DebugOut(L"[_ParseSection_SCENES] %s\n", ToWSTR(line).c_str());

	if (tokens.size() < 3) return;
	int id = atoi(tokens[0].c_str());
	int type = atoi(tokens[1].c_str());
	LPCWSTR path = ToLPCWSTR(tokens[2]);	// file: ASCII format (single-byte char) => Wide Char

	LPSCENE scene = NULL;
	switch (type)
	{
	case 0: // Intro
		scene = new WorldScene(id, path);
		break;
	case 1: // Play Scene
		scene = new PlayScene(id, path);
		break;
	}
	if (scene != NULL)
	{
		SceneManager::GetInstance()->AddScene(id, scene);
	}
}

void Game::_ParseSection_TEXTURES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return;

	int texID = atoi(tokens[0].c_str());
	wstring path = ToWSTR(tokens[1]);

	Textures::GetInstance()->Add(texID, path.c_str());
}

void Game::_ParseSection_AUDIO(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return;

	string soundId = tokens[0];
	string soundPath = tokens[1];

	DebugOut(L"[AUDIO LOAD] ID: %s | Path: %s\n", ToWSTR(soundId).c_str(), ToWSTR(soundPath).c_str());
	SoundManager::GetInstance()->Load(soundId, soundPath);
}


Game::~Game()
{
	if (GameGlobal::pBlendStateAlpha) GameGlobal::pBlendStateAlpha->Release();
	if (GameGlobal::spriteObject) GameGlobal::spriteObject->Release();
	if (GameGlobal::pRenderTargetView) GameGlobal::pRenderTargetView->Release();
	if (GameGlobal::pSwapChain) GameGlobal::pSwapChain->Release();
	if (GameGlobal::pD3DDevice) GameGlobal::pD3DDevice->Release();
}


/*
	Update world status for this frame
	dt: time period between beginning of last frame and beginning of this frame
*/
void Game::Update(DWORD dt)
{
	InputManager::GetInstance()->ProcessKeyboard();
	LPSCENE currentScene = SceneManager::GetInstance()->GetCurrentScene();
	if (currentScene != NULL)
	{
		currentScene->Update(dt);
	}
	else
	{
		DebugOut(L"[WARNING] Không tìm thấy Current Scene để Update!\n");
	}
}


/*
	Render a frame
*/
void Game::Render()
{
	ID3D10Device* pD3DDevice = GameGlobal::pD3DDevice;
	IDXGISwapChain* pSwapChain = GameGlobal::pSwapChain;
	ID3D10RenderTargetView* pRenderTargetView = GameGlobal::pRenderTargetView;
	ID3DX10Sprite* spriteHandler = GameGlobal::spriteObject;
	ID3D10BlendState* pBlendStateAlpha = GameGlobal::pBlendStateAlpha;

	LPSCENE currentScene = SceneManager::GetInstance()->GetCurrentScene();
	if (currentScene != NULL)
	{
		pD3DDevice->ClearRenderTargetView(pRenderTargetView, currentScene->GetBackgroundColor());
	}
	else
	{
		// Màu dự phòng nếu game bị lỗi chưa có Scene
		float fallbackColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		pD3DDevice->ClearRenderTargetView(pRenderTargetView, fallbackColor);
	}

	// 3. Bắt đầu vẽ các vật thể
	spriteHandler->Begin(D3DX10_SPRITE_SORT_TEXTURE);

	FLOAT NewBlendFactor[4] = { 0,0,0,0 };
	pD3DDevice->OMSetBlendState(pBlendStateAlpha, NewBlendFactor, 0xffffffff);

	if (currentScene != NULL)
	{
		currentScene->Render();
	}
	else
	{
		DebugOut(L"[WARNING] Không tìm thấy Current Scene để Render \n");
	}

	spriteHandler->End();
	pSwapChain->Present(0, 0);
}
