#pragma once

#include "Singleton.h"

class Camera : public Singleton<Camera> {
	friend class Singleton<Camera>;

private:
	float cam_x = 0.0f;
	float cam_y = 0.0f;

public:
	void SetCamPos(float x, float y) { cam_x = x; cam_y = y; }
	void GetCamPos(float& x, float& y) { x = cam_x; y = cam_y; }
	
	float GetCamX() { return cam_x; }
};
