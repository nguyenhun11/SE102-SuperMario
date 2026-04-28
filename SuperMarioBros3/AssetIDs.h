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



#define OBJECT_TYPE_MARIO	0
#define OBJECT_TYPE_BRICK	1
#define OBJECT_TYPE_GOOMBA	2
#define OBJECT_TYPE_KOOPAS	3
#define OBJECT_TYPE_COIN 4
#define OBJECT_TYPE_PLATFORM 7
#define OBJECT_TYPE_QUESTION_BLOCK	302

#define OBJECT_TYPE_PORTAL	50
