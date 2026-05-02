#pragma once

/*
ID CONVENTION
Each ID for a type of object will BEGIN with:
	1. Player
	2. Enemies
	3. Blocks
	4. Items
	5. Weapons, effects
	6. Play Scene (ground, background,...)
	7. Platforms
	8. Other scene (intro, select level,...)
	9. UI, other
*/

#define ID_TEX_MARIO 0
#define ID_TEX_ENEMY 10
#define ID_TEX_MISC 20

/*
	Các bước thêm mới:
	1. define trong đây con số nào cũng được, không trùng những cái trước
	2. Đổi tên file gốc để dễ biết
	3. Thêm số tương ứng vào game-data.txt

*/
#define ID_TEXTURE_MARIO			100
#define ID_TEXTURE_ENEMIES			200
#define ID_TEXTURE_TILESETS_STAGE	300
#define ID_TEXTURE_ITEMS			400


// Characters
#define OBJECT_TYPE_MARIO	0
#define OBJECT_TYPE_GOOMBA	2
#define OBJECT_TYPE_KOOPA	202

#define OBJECT_TYPE_PLATFORM 7

// Object


// Block
#define OBJECT_TYPE_BRICK	301
#define OBJECT_TYPE_QUESTION_BLOCK	302


// Map
#define OBJECT_TYPE_BACKGROUND 600
#define OBJECT_TYPE_GROUND 601

//Platform
#define OBJECT_TYPE_SEMISOLID_PLATFORM 701
#define OBJECT_TYPE_VERTICAL_PIPE 702
#define OBJECT_TYPE_HORIZOLTAL_PIPE 703 //Chua lam

// Objects
#define OBJECT_TYPE_COIN 401
#define OBJECT_TYPE_MUSHROOM 402
#define OBJECT_TYPE_LEAF	403
#define OBJECT_TYPE_ONEUPMUSHROOM	404

// Others
#define OBJECT_TYPE_PORTAL	50

#define SCENE_HUD 901
