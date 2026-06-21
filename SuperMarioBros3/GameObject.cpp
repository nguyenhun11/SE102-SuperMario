#include <d3dx9.h>
#include <algorithm>


#include "debug.h"
#include "Textures.h"
#include "Game.h"
#include "GameObject.h"
#include "Sprites.h"

GameObject::GameObject()
{
	x = y = 0;
	vx = vy = 0;
	nx = 1;	
	state = -1;
	zIndex = 0;
	isDeleted = false;
}

bool GameObject::IsOnCamera()
{
	float camX, camY;
	Camera::GetInstance()->GetCamPos(camX, camY);

	float camLeft = camX;
	float camTop = camY;
	float camRight = camLeft + GameGlobal::GetWidth();
	float camBottom = camTop + GameGlobal::GetHeight();

	float objLeft = 0, objTop = 0, objRight = 0, objBottom = 0;
	GetBoundingBox(objLeft, objTop, objRight, objBottom);

	float pad = 80.0f;
	if (objLeft == 0 && objTop == 0 && objRight == 0 && objBottom == 0)
	{
		return (x >= camLeft - pad && x <= camRight + pad &&
			y >= camTop - pad && y <= camBottom + pad);
	}

	return !(objRight < camLeft - pad || objLeft > camRight + pad || objBottom < camTop - pad || objTop > camBottom + pad);
}

void GameObject::CheckCameraStatus()
{
	bool currentlyOnCamera = IsOnCamera();

	if (currentlyOnCamera && !wasOnCamera) {
		OnEnterCamera();
	}
	else if (!currentlyOnCamera && wasOnCamera) {
		OnExitCamera();
	}

	wasOnCamera = currentlyOnCamera;
}

void GameObject::RenderBoundingBox()
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPTEXTURE bbox = Textures::GetInstance()->Get(ID_TEX_BBOX);

	float l,t,r,b; 

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	float cx, cy; 
	Camera::GetInstance()->GetCamPos(cx, cy);

	Game::GetInstance()->Draw(x - cx, y - cy, bbox, &rect, BBOX_ALPHA);
}

GameObject::~GameObject()
{

}