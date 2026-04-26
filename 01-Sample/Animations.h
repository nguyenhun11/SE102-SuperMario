#pragma once

#include "Animation.h"
#include "Sprite.h"
#include "Singleton.h"

class Animations : public Singleton<Animations>
{
	friend class Singleton<Animations>;

private:
	unordered_map<int, LPANIMATION> animations;

public:
	void Add(int id, LPANIMATION ani);
	LPANIMATION Get(int id);
	void Clear();
};