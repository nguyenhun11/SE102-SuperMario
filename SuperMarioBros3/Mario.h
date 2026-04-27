#pragma once
#include "GameObject.h"

#include "Animation.h"
#include "Animations.h"

#include "debug.h"

// Moving Speed
#define MARIO_WALKING_SPEED		0.12f
#define MARIO_RUNNING_SPEED		0.2f

// Accelaration
#define MARIO_ACCEL_WALK_X	0.0004f
#define MARIO_ACCEL_RUN_X	0.0008f
#define MARIO_ACCEL_SKID	0.0002f

// Deccelation
#define MARIO_DECCEL_WALK_X	0.0002f
#define MARIO_DECCEL_RUN_X	0.0004f

// Jump speed
#define MARIO_JUMP_SPEED_Y		0.35f
#define MARIO_JUMP_RUN_SPEED_Y	0.4f

// Gravity
#define MARIO_GRAVITY			0.0007f

// Bounce force
#define MARIO_JUMP_DEFLECT_SPEED  0.2f
#define MARIO_HIGH_JUMP_DEFLECT_SPEED 0.4f


// ------------------------- MARIO STATE -------------------------------- //
#pragma	region	MARIO_STATES & MARIO_FORMS
enum class MarioState 
{
	DIE = 99,
	IDLE = 0,
	WALKING = 1,
	RUNNING = 2,
	SKIDDING = 3,
	JUMP = 4,
	FALIING = 6,
	SIT = 7,
	SLIDING = 8,
	PIPING = 9,
	HOLDING = 10,
	KICKING = 11,
	GOAL = 12,
	FLYING = 13,
	FLOATING = 14,
	SPIN_TAIL = 15,
	SHOOT_FIRE = 16,

	JUMP_RELEASE = 80,
	SIT_RELEASE = 81
};

enum class MarioForm
{
	SMALL = 0,
	SUPER = 1,
	FIRE = 2,
	RACOON = 3
};

#pragma	endregion


// ------------------------- MARIO ANIMATION -------------------------------- //
#pragma region ANIMATION_ID

// SUPER MARIO
#define ID_ANI_MARIO_SUPER_IDLE 1100
#define ID_ANI_MARIO_SUPER_SKIDDING 1103

#define ID_ANI_MARIO_SUPER_WALKING 1101
#define ID_ANI_MARIO_SUPER_RUNNING 1102

#define ID_ANI_MARIO_SUPER_JUMP_WALK 1104
#define ID_ANI_MARIO_SUPER_JUMP_RUN 1105
#define ID_ANI_MARIO_SUPER_FALLING	1106

#define ID_ANI_MARIO_SUPER_SIT 1107

#define ID_ANI_MARIO_DIE 999

// SMALL MARIO
#define ID_ANI_MARIO_SMALL_IDLE 1000
#define ID_ANI_MARIO_SMALL_SKIDDING 1003

#define ID_ANI_MARIO_SMALL_WALKING 1001
#define ID_ANI_MARIO_SMALL_RUNNING 1002


#define ID_ANI_MARIO_SMALL_JUMP_WALK 1004
#define ID_ANI_MARIO_SMALL_JUMP_RUN 1005

// Other Game feel Stuff
#define	MARIO_DIE_TIMEOUT	800
#define MARIO_DIE_BOUNCE_FORCE	0.25f
#define	MARIO_DIE_GRAVITY	0.0005f

#pragma endregion


#define GROUND_Y 160.0f

// ------------------------- BOUNDING SHAPES -------------------------------- //
#define MARIO_BIG_BBOX_WIDTH  14
#define MARIO_BIG_BBOX_HEIGHT 24
#define MARIO_BIG_SITTING_BBOX_WIDTH  14
#define MARIO_BIG_SITTING_BBOX_HEIGHT 16

#define MARIO_SIT_HEIGHT_ADJUST ((MARIO_BIG_BBOX_HEIGHT-MARIO_BIG_SITTING_BBOX_HEIGHT)/2)

#define MARIO_SMALL_BBOX_WIDTH  13
#define MARIO_SMALL_BBOX_HEIGHT 12


#define MARIO_UNTOUCHABLE_TIME 2500

class Mario : public GameObject
{
	BOOLEAN isSitting;
	float maxVx;
	float accelX;				// acceleration on x 
	float accelY;				// acceleration on y 

	MarioForm level; 
	int untouchable; 
	ULONGLONG die_start;
	ULONGLONG untouchable_start;
	BOOLEAN isOnPlatform;
	int coin; 

	void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithCoin(LPCOLLISIONEVENT e);
	void OnCollisionWithPortal(LPCOLLISIONEVENT e);

	int GetAniIdBig();
	int GetAniIdSmall();

	MarioState currentState;


public:
	Mario(float x, float y) : GameObject(x, y)
	{
		isSitting = false;
		maxVx = 0.0f;
		accelX = 0.0f;
		accelY = MARIO_GRAVITY; 

		level = MarioForm::SUPER;
		untouchable = 0;
		untouchable_start = -1;
		die_start = -1;
		isOnPlatform = false;
		coin = 0;

		currentState = MarioState::IDLE;
	}

	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	void SetState(MarioState state);
	void SetDirection(int d) { nx = d; }

	int IsCollidable()
	{ 
		return (state != static_cast<int>(MarioState::DIE));
	}

	int IsBlocking()
	{
		return (state != static_cast<int>(MarioState::DIE) && untouchable == 0);
	}

	bool IsHoldingJump = false;

	void OnNoCollision(DWORD dt);
	void OnCollisionWith(LPCOLLISIONEVENT e);
	void OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e);

	void SetLevel(MarioForm form);
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount64(); }

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);
};