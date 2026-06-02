#pragma once
#include "GameObject.h"
#include "QuestionBlock.h" 

#define ID_ANI_NOTE_BLOCK_ACTIVE 3030

#define NOTE_BLOCK_BBOX_WIDTH 16
#define NOTE_BLOCK_BBOX_HEIGHT 16

#define NOTE_BLOCK_BOUNCE_SPEED 0.15f
#define NOTE_BLOCK_BOUNCE_HEIGHT 8.0f

enum class NoteBlockState 
{
    ACTIVE = 0,
    BOUNCING_UP = 1,   
    BOUNCING_DOWN = 2, 
    EMPTY = 3          
};

class NoteBlock : public GameObject
{
private:
    float startY;
    ContainedItem item;
    NoteBlockState currentState;
	int bounceCount;

public:
    NoteBlock(float x, float y, int containedItem = 0, int count = 1);

    virtual void Render();
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
    virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

    void SetState(NoteBlockState state);
    int IsCollidable() override { return 1; }
    int IsBlocking() override { return 1; }

    void SpawnItem();
    void MarioJumpDeflect();

    NoteBlockState GetNoteBlockState() { return currentState; }
};