#include "Animation.h"
#include "debug.h"

void Animation::Add(int spriteId, DWORD time)
{
	int t = time;
	if (time == 0) t = this->defaultTime;

	LPSPRITE sprite = Sprites::GetInstance()->Get(spriteId);
	if (sprite == NULL)
		DebugOut(L"[ERROR] Sprite ID %d not found!\n", spriteId);

	LPANIMATION_FRAME frame = new AnimationFrame(sprite, t);
	frames.push_back(frame);
}

void Animation::Render(float x, float y, bool isFlip, bool isFlipVertical)
{
	if (frames.size() == 0) return;
	ULONGLONG now = GetTickCount64();
	if (currentFrame == -1)
	{
		currentFrame = 0;
		lastFrameTime = now;
	}
	else
	{
		DWORD t = frames[currentFrame]->GetTime();
		if (now - lastFrameTime > t)
		{
			currentFrame++;
			lastFrameTime = now;
			if (currentFrame == frames.size()) currentFrame = 0;
		}

	}

	//frames[currentFrame]->GetSprite()->DrawOnCamera(x, y, isFlip, isFlipVertical);
	LPSPRITE currentSprite = frames[currentFrame]->GetSprite();
	if (currentSprite != NULL)
	{
		currentSprite->DrawOnCamera(x, y, isFlip, isFlipVertical);
	}
	else
	{
		DebugOut(L"[ERROR] Frame thu %d bi rỗng Sprite! \n", currentFrame);
	}
}

