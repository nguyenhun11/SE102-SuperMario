#pragma once
#include "GameObject.h"

#include "Animation.h"
#include "Animations.h"
#include "GameManager.h"
#include "debug.h"

// Moving Speed
#define MARIO_WALKING_SPEED		0.1f
#define MARIO_RUNNING_SPEED		0.18f

// Accelaration
#define MARIO_ACCEL_WALK_X	0.0003f
#define MARIO_ACCEL_RUN_X	0.0005f
#define MARIO_ACCEL_SKID	0.00015f

// Deccelation
#define MARIO_DECCEL_WALK_X	0.0002f
#define MARIO_DECCEL_RUN_X	0.0004f

//// Jump speed
//#define MARIO_JUMP_SPEED_Y		0.35f
//#define MARIO_JUMP_RUN_SPEED_Y	0.4f
//
//// Gravity
//#define MARIO_GRAVITY			0.0007f

#define MARIO_JUMP_SPEED_Y		0.3f
#define MARIO_SMALL_JUMP_RUN_SPEED_Y	0.35f
#define MARIO_JUMP_RUN_SPEED_Y	0.4f

// Gravity
#define MARIO_GRAVITY			0.0006f

// Bounce force
#define MARIO_JUMP_DEFLECT_SPEED  0.2f
#define MARIO_HIGH_JUMP_DEFLECT_SPEED 0.35f

// Floating & flying speed
#define MARIO_FLYING_TIME 4000    
#define MARIO_FLOATING_TIME	250
#define MARIO_SLOW_FALL_SPEED 0.01f 
#define MARIO_FLYING_UP_FORCE 0.25f

// Tail attack 
#define MARIO_SPIN_TIME	240

// P-meter
#define MARIO_PMETER_MAX 7
#define MARIO_PMETER_CHARGE_TIME 200 

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
	RACOON = 2,
	FIRE = 3
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

// RACOON MARIO
#define ID_ANI_MARIO_RACOON_IDLE 1200
#define ID_ANI_MARIO_RACOON_SKIDDING 1203

#define ID_ANI_MARIO_RACOON_WALKING 1201
#define ID_ANI_MARIO_RACOON_RUNNING 1202

#define ID_ANI_MARIO_RACOON_JUMP_WALK 1204
#define ID_ANI_MARIO_RACOON_JUMP_RUN 1205
#define ID_ANI_MARIO_RACOON_FALLING	1206

#define ID_ANI_MARIO_RACOON_SIT 1207
#define ID_ANI_MARIO_RACOON_SPIN 1215
// NOTE NÈ: thiếu slow fall với flying

#define ID_ANI_MARIO_RACOON_FLYING 1213
#define ID_ANI_MARIO_RACOON_FLOATING 1214


// ---- Other Game feel Stuff ----
// Die Animation
#define	MARIO_DIE_TIMEOUT	800
#define MARIO_DIE_BOUNCE_FORCE	0.09f
#define	MARIO_DIE_GRAVITY	0.00008f

// Hit Animation
#define MARIO_HIT_TIMEOUT	1000
#pragma endregion

// Transform Animation
#define MARIO_TRANSFORM_SUPER_TIME 1000
#define MARIO_TRANSFORM_TIME 500

// Poof Transform
#define MARIO_POOF_TIME 320
#define ID_ANI_MARIO_POOF 1980

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
	
	bool isSuperTransforming;
	bool isTakingDamage;
	bool canFly;
	bool isFlying;
	bool isFloating;
	bool isPoofTransforming;
	bool isSpinning;
	
	// pmeter
	int pmeter;

	MarioForm form; 
	int untouchable; 

	ULONGLONG die_start;
	ULONGLONG damage_start;
	ULONGLONG untouchable_start;
	ULONGLONG transform_start;
	ULONGLONG fly_start;
	ULONGLONG flap_start;
	ULONGLONG poof_start;
	ULONGLONG spin_start;
	ULONGLONG pmeter_start;

	MarioForm nextPoofForm;
	BOOLEAN isOnPlatform;
	//int coin; 
	//int score;

	void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithCoin(LPCOLLISIONEVENT e);
	void OnCollisionWithPortal(LPCOLLISIONEVENT e);
	void OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithMushroom(LPCOLLISIONEVENT e);
	void OnCollisionWithLeaf(LPCOLLISIONEVENT e);
	void OnCollisionWithBrick(LPCOLLISIONEVENT e);

	int GetAniIdBig();
	int GetAniIdSmall();
	int GetAniIdRacoon();

	MarioState currentState;


public:
	Mario(float x, float y) : GameObject(x, y)
	{
		isSitting = false;
		maxVx = 0.0f;
		accelX = 0.0f;
		accelY = MARIO_GRAVITY; 
		pmeter = 0;

		isOnPlatform = false;
		isSuperTransforming = false;
		isTakingDamage = false;
		canFly = false;
		isFlying = false;
		isFloating = false;
		isPoofTransforming = false;
		isSpinning = false;

		untouchable = 0;
		untouchable_start = -1;
		die_start = -1;
		damage_start = -1;
		transform_start = -1;
		fly_start = -1;
		flap_start = -1;
		poof_start = -1;
		spin_start = -1;
		pmeter_start = -1;

		//coin = 0;
		//score = 0;

		form = MarioForm::SMALL;
		currentState = MarioState::IDLE;
		nextPoofForm = MarioForm::RACOON;

		zIndex = 10;
	}

	void AddCoin(int amount = 1)
	{
		//coin += amount;
		GameManager::GetInstance()->AddCoin(amount);
		//DebugOut(L">>> CurrentCoin: %d\n", coin);
	}

	void AddScore(int amount = 100)
	{
		//score += amount;
		GameManager::GetInstance()->AddScore(amount);
		//DebugOut(L">>> CurrentScore: %d\n", score);
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
	

	void SetNewForm(MarioForm form);
	void StartUntouchable() { untouchable = 1; untouchable_start = GetTickCount64(); }

	void GetBoundingBox(float& left, float& top, float& right, float& bottom);

	// Transformation
	void StartTransform();
	void StartPoofTransform(MarioForm targetForm);

	// Behaviours
	void Attack();
	void TakeDamage();

	// Handle Update
	void HandleDying(DWORD dt, vector<LPGAMEOBJECT>* coObjects); 
	void HandleTakingDamage(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandleSpinning(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandleTransform(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandlePMeter(DWORD dt, vector<LPGAMEOBJECT>* coObjects);

	// Getters & Setters
	float GetX() { return x; }
	float GetY() { return y; }
	MarioForm GetCurrentForm() { return form; }
	int GetPMeter() { return pmeter; }
};