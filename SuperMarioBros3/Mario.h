#pragma once
#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"
#include "debug.h"
#include "VerticalPipe.h"
#include "HorizontalPipe.h"

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
#define MARIO_FLYING_UP_FORCE 0.2f

// Tail attack 
#define MARIO_SPIN_TIME	240

// P-meter
#define MARIO_PMETER_MAX 7
#define MARIO_PMETER_CHARGE_TIME 200 

// Piping
#define MARIO_PIPE_TIME 1400
#define MARIO_PIPE_SPEED 0.02f

// Kicking
#define MARIO_KICK_TIME 200

class Koopa;

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

// SMALL MARIO
#define ID_ANI_MARIO_SMALL_IDLE 1000
#define ID_ANI_MARIO_SMALL_SKIDDING 1003

#define ID_ANI_MARIO_SMALL_WALKING 1001
#define ID_ANI_MARIO_SMALL_RUNNING 1002
#define ID_ANI_MARIO_SMALL_SLIDING 1008

#define ID_ANI_MARIO_SMALL_JUMP_WALK 1004
#define ID_ANI_MARIO_SMALL_JUMP_RUN 1005

#define ID_ANI_MARIO_SMALL_PIPING 1009

#define ID_ANI_MARIO_SMALL_HOLDING_IDLE	1020
#define ID_ANI_MARIO_SMALL_HOLDING_RUN	1021
#define ID_ANI_MARIO_SMALL_HOLDING_JUMP	1022

#define ID_ANI_MARIO_SMALL_KICKING	1011

// SUPER MARIO
#define ID_ANI_MARIO_SUPER_IDLE 1100
#define ID_ANI_MARIO_SUPER_SKIDDING 1103
#define ID_ANI_MARIO_SUPER_SKIDDING 1103

#define ID_ANI_MARIO_SUPER_WALKING 1101
#define ID_ANI_MARIO_SUPER_RUNNING 1102

#define ID_ANI_MARIO_SUPER_JUMP_WALK 1104
#define ID_ANI_MARIO_SUPER_JUMP_RUN 1105
#define ID_ANI_MARIO_SUPER_FALLING	1106

#define ID_ANI_MARIO_SUPER_SIT 1107
#define ID_ANI_MARIO_SUPER_SLIDING 1108

#define ID_ANI_MARIO_SUPER_PIPING 1109

#define ID_ANI_MARIO_DIE 999

#define ID_ANI_MARIO_SUPER_HOLDING_IDLE	1120
#define ID_ANI_MARIO_SUPER_HOLDING_RUN	1121
#define ID_ANI_MARIO_SUPER_HOLDING_JUMP	1122

#define ID_ANI_MARIO_SUPER_KICKING	1111

// RACOON MARIO
#define ID_ANI_MARIO_RACOON_IDLE 1200
#define ID_ANI_MARIO_RACOON_SKIDDING 1203

#define ID_ANI_MARIO_RACOON_WALKING 1201
#define ID_ANI_MARIO_RACOON_RUNNING 1202

#define ID_ANI_MARIO_RACOON_JUMP_WALK 1204
#define ID_ANI_MARIO_RACOON_JUMP_RUN 1205
#define ID_ANI_MARIO_RACOON_FALLING	1206

#define ID_ANI_MARIO_RACOON_SIT 1207
#define ID_ANI_MARIO_RACOON_SLIDING 1208
#define ID_ANI_MARIO_RACOON_SPIN 1215

#define ID_ANI_MARIO_RACOON_FLYING 1213
#define ID_ANI_MARIO_RACOON_FLOATING 1214

#define ID_ANI_MARIO_RACOON_PIPING 1209

#define ID_ANI_MARIO_RACOON_HOLDING_IDLE	1220
#define ID_ANI_MARIO_RACOON_HOLDING_RUN	1221
#define ID_ANI_MARIO_RACOON_HOLDING_JUMP	1222

#define ID_ANI_MARIO_RACOON_KICKING	1211

// ---- Other Game feel Stuff ----
// Die Animation
#define	MARIO_DIE_TIMEOUT	800
#define MARIO_DIE_BOUNCE_FORCE	0.09f
#define	MARIO_DIE_GRAVITY	0.00008f

// Hit Animation
#define MARIO_HIT_TIMEOUT	1000
#pragma endregion

// Transform Animation
#define MARIO_TRANSFORM_SUPER_TIME 900
#define MARIO_TRANSFORM_TIME 500

// Poof Transform
#define MARIO_POOF_TIME 320
#define ID_ANI_MARIO_POOF 1980

#define GROUND_Y 160.0f

// ------------------------- BOUNDING SHAPES -------------------------------- //
#define MARIO_BIG_BBOX_WIDTH  10
#define MARIO_BIG_BBOX_HEIGHT 24
#define MARIO_BIG_SITTING_BBOX_WIDTH  10
#define MARIO_BIG_SITTING_BBOX_HEIGHT 12

#define MARIO_SIT_HEIGHT_ADJUST ((MARIO_BIG_BBOX_HEIGHT-MARIO_BIG_SITTING_BBOX_HEIGHT)/2)

#define MARIO_SMALL_BBOX_WIDTH  10
#define MARIO_SMALL_BBOX_HEIGHT 12


#define MARIO_UNTOUCHABLE_TIME 2500

class Mario : public GameObject
{
	BOOLEAN isSitting;
	float maxVx;
	float accelX;				// acceleration on x 
	float accelY;				// acceleration on y 

	// original pos
	float start_x;
	float start_y;
	float slopeDirection;

	bool isSuperTransforming;
	bool isTakingDamage;
	bool canFly;
	bool wasSkidding;
	bool isFlying;
	bool isFloating;
	bool isPoofTransforming;
	bool isSpinning;
	bool isSliding;
	bool isPiping;
	bool isKicking;
	bool isDieBounce;

	bool isGoalRunning;

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
	ULONGLONG pmeter_sound_start;
	ULONGLONG piping_start;
	ULONGLONG kick_start;

	MarioForm nextPoofForm;
	BOOLEAN isOnPlatform;
	BOOLEAN isOnSlope;

	VerticalPipe* pipeBelow;
	VerticalPipe* pipeAbove;
	bool isPipingUp = false;
	bool isPipingHorizontal = false;
	int pipingDirectionX = 0; // 1: chui qua phải, -1: chui qua trái
	HorizontalPipe* activeHorizontalPipe = nullptr;

	void OnCollisionWithGoalBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
	void OnCollisionWithKoopa(LPCOLLISIONEVENT e);
	void OnCollisionWithPiranhaPlant(LPCOLLISIONEVENT e);
	void OnCollisionWithFire(LPCOLLISIONEVENT e);
	void OnCollisionWithBoomerangBro(LPCOLLISIONEVENT e);
	void OnCollisionWithBoomerang(LPCOLLISIONEVENT e);
	void OnCollisionWithCoin(LPCOLLISIONEVENT e);
	void OnCollisionWithPortal(LPCOLLISIONEVENT e);
	void OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithWoodBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithInvisibleBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithWoodBlockSensor(LPCOLLISIONEVENT e);
	void OnCollisionWithOneUpMushroom(LPCOLLISIONEVENT e);
	void OnCollisionWithMushroom(LPCOLLISIONEVENT e);
	void OnCollisionWithLeaf(LPCOLLISIONEVENT e);
	void OnCollisionWithBrick(LPCOLLISIONEVENT e);
	void OnCollisionWithNoteBlock(LPCOLLISIONEVENT e);
	void OnCollisionWithVerticalPipe(LPCOLLISIONEVENT e);
	void OnCollisionWithHorizontalPipe(LPCOLLISIONEVENT e);
	void OnCollisionWithLift(LPCOLLISIONEVENT e);


	int GetAniIdBig();
	int GetAniIdSmall();
	int GetAniIdRacoon();

	MarioState currentState;


public:
	bool isFlyingToHeaven;
	int heavenSceneID;


	Mario(float x, float y) : GameObject(x, y)
	{
		start_x = x;
		start_y = y;
		slopeDirection = 1;
		isSitting = false;
		maxVx = 0.0f;
		accelX = 0.0f;
		accelY = MARIO_GRAVITY;
		pmeter = 0;

		isOnPlatform = false;
		isOnSlope = false;
		isSuperTransforming = false;
		isTakingDamage = false;
		canFly = false;
		wasSkidding = false;
		isFlying = false;
		isFloating = false;
		isPoofTransforming = false;
		isSpinning = false;
		isSliding = false;
		isPiping = false;
		isPipingUp = false;
		isKicking = false;
		isDieBounce = false;

		isGoalRunning = false;
		isFlyingToHeaven = false;
		heavenSceneID = -1;

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
		piping_start = -1;
		kick_start = -1;

		pipeBelow = NULL;

		//coin = 0;
		//score = 0;

		SetUp();

		currentState = MarioState::IDLE;
		nextPoofForm = MarioForm::RACOON;

		zIndex = 10;
	}

	void SetUp();

	void AddCoin(int amount = 1);

	void AddScore(int amount = 100);

	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void Render();
	void SetState(MarioState state);
	void SetDirection(int d);

	int IsCollidable()
	{
		return (state != static_cast<int>(MarioState::DIE) && state != static_cast<int>(MarioState::PIPING));
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
	void Reset();
	void EnterPipeDown();
	void EnterPipeUp();
	void SetStartPiping();

	// Handle Update
	void HandleDying(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandleTakingDamage(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandleSpinning(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandleTransform(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandlePMeter(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandleSlope(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandleSlopePhysics(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandleGoalRunning(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandlePiping(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandleHolding(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void HandleKicking(DWORD dt, vector<LPGAMEOBJECT>* coObjects);

	// Getters & Setters
	float GetX() { return x; }
	float GetY() { return y; }
	MarioForm GetCurrentForm() { return form; }
	int GetPMeter() { return pmeter; }
	bool IsGoalRunning() { return isGoalRunning; }
	int GetDirection() { return nx; }
	bool IsOnPlatform() { return isOnPlatform; }
	void SetIsOnPlatform(bool on) { isOnPlatform = on; }
	void StartFlyingToHeaven(int sceneID);
	void HandleFlyingToHeaven(DWORD dt, vector<LPGAMEOBJECT>* coObjects);

	bool isHolding = false;
	Koopa* heldKoopa = NULL;
};