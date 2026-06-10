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

// Characters
#define OBJECT_TYPE_MARIO			0
#define OBJECT_TYPE_GOOMBA			2
#define OBJECT_TYPE_KOOPA			202
#define OBJECT_TYPE_RED_KOOPA		212
#define OBJECT_TYPE_PIRANHA_PLANT   203

// Block
#define OBJECT_TYPE_SOLID_BLOCK		300
#define OBJECT_TYPE_BRICK			301
#define OBJECT_TYPE_QUESTION_BLOCK	302
#define OBJECT_TYPE_NOTE_BLOCK		303
#define OBJECT_TYPE_GOAL_BLOCK		309

// Objects
#define OBJECT_TYPE_COIN			401
#define OBJECT_TYPE_MUSHROOM		402
#define OBJECT_TYPE_LEAF			403
#define OBJECT_TYPE_ONEUPMUSHROOM	404
#define OBJECT_TYPE_SWITCH			405

// Map
#define OBJECT_TYPE_DECORATION 600
#define OBJECT_TYPE_GROUND 601
#define OBJECT_TYPE_SLOPE 603

//Platform
#define OBJECT_TYPE_PLATFORM			7
#define OBJECT_TYPE_SEMISOLID_PLATFORM	701
#define OBJECT_TYPE_VERTICAL_PIPE		702
#define OBJECT_TYPE_HORIZOLTAL_PIPE		703

//Worldmap
#define WORLDMAP_MAP		800
#define WORLDMAP_MARIO		801
#define WORLDMAP_OBJECT		802
#define WORLDMAP_NODE_INVISIBLE		80300
#define WORLDMAP_NODE_STAGE			80301




// Others
#define OBJECT_TYPE_PORTAL	50
