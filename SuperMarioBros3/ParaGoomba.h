#pragma once
#include "Goomba.h"

// ==== Vận tốc & chu kỳ bay ====
#define PARAGOOMBA_WALKING_SPEED   0.02f
#define PARAGOOMBA_GRAVITY         GOOMBA_GRAVITY

#define PARAGOOMBA_WALK_DURATION   1500      // ms đi bộ trước khi bắt đầu nhảy
#define PARAGOOMBA_HOP_COUNT       3         // số lần nhảy tưng thấp
#define PARAGOOMBA_HOP_VY          -0.15f    // vận tốc nhảy thấp
#define PARAGOOMBA_JUMP_VY         -0.25f    // vận tốc nhảy vọt cao

// ==== ID Animation ====
#define ID_ANI_PARAGOOMBA_WALKING  20110
#define ID_ANI_WING_FLAP           20120
#define ID_ANI_PARAGOOMBA_DIE      20130
#define ID_ANI_PARAGOOMBA_BOUNCE   20140

#define PARAGOOMBA_HOP_PAUSE   250   // ms nghỉ trên đất giữa mỗi lần hop
#define PARAGOOMBA_JUMP_PAUSE  300

enum class FlightPhase
{
	WALK = 0,
	HOP = 1,
	JUMP = 2
};

class ParaGoomba : public Goomba
{
protected:
	bool hasWings = true;

	FlightPhase flightPhase = FlightPhase::WALK;
	ULONGLONG phaseStart = 0;
	int hopDoneCount = 0;

	// Edge-triggered landing detection, KHÔNG dùng time-debounce nữa
	bool isAirborne = false;  // true = đang bay (vừa hop/jump, chưa chạm đất lại)
	bool justLanded = false;  // cờ 1-shot: vừa chạm đất, chưa được xử lý

	ULONGLONG groundPauseStart = 0;
	bool isPausing = false;

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
	virtual void Render() override;

	virtual void OnCollisionWith(LPCOLLISIONEVENT e) override;

	void UpdateFlightPattern(DWORD dt);
	void StartPhase(FlightPhase phase);
	void FaceTowardPlayer();

public:
	ParaGoomba(float x, float y);

	virtual void OnEnable() override;
	virtual void SetState(GoombaState state) override;

	virtual void OnStompedByMario();
	virtual void OnHitSideways();

	bool HasWings() const { return hasWings; }
};