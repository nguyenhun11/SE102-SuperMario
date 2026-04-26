#pragma once
#include <dinput.h>
#include "GameGlobal.h"
#include "debug.h"
#include "KeyEventHandler.h"
#include "Singleton.h"

#define MAX_FRAME_RATE 100
#define KEYBOARD_BUFFER_SIZE 1024
#define KEYBOARD_STATE_SIZE 256
#define DIRECTINPUT_VERSION 0x0800

class InputManager : public Singleton<InputManager> {
	friend class Singleton<InputManager>;

private:
	LPDIRECTINPUT8 di;
	LPDIRECTINPUTDEVICE8 didv;
	BYTE  keyStates[KEYBOARD_STATE_SIZE];
	DIDEVICEOBJECTDATA keyEvents[KEYBOARD_BUFFER_SIZE];
	LPKEYEVENTHANDLER keyHandler;

public:
	void InitKeyboard();
	int IsKeyDown(int KeyCode);
	void ProcessKeyboard();
	void SetKeyHandler(LPKEYEVENTHANDLER handler) { keyHandler = handler; }
};