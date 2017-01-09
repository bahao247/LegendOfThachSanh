#pragma once
#ifndef __SdkTableCommon_H__
#define __SdkTableCommon_H__
#pragma region [Define Terrain]
#define TERRAIN_PAGE_MIN_X 0
#define TERRAIN_PAGE_MIN_Y 0
#define TERRAIN_PAGE_MAX_X 0
#define TERRAIN_PAGE_MAX_Y 0
#define TERRAIN_FILE_PREFIX String("terrain_level3")
#define TERRAIN_FILE_SUFFIX String("dat")
#define TERRAIN_WORLD_SIZE 12000.0f
#define TERRAIN_SIZE 513
#define MAX_GRASS 11
#define MAX_GROUP_GRASS 3

enum Mode
{
	MODE_NORMAL = 0,
	MODE_EDIT_HEIGHT = 1,
	MODE_EDIT_BLEND = 2,
	MODE_COUNT = 3
};
enum ShadowMode
{
	SHADOWS_NONE = 0,
	SHADOWS_COLOUR = 1,
	SHADOWS_DEPTH = 2,
	SHADOWS_COUNT = 3
};
enum GrassType
{
	GRASS_NONE = 0,
	GRASS_RED_FLOWER = 1,
	GRASS_YELLOW_FLOWER = 2,
	GRASS_ALOE_VERA = 3,
	GRASS_DIEP = 4,
	GRASS_DAO_FLOWER = 5,
	GRASS_ANH_TUC = 6,
	GRASS_MAI = 7,
	GRASS_GANG = 8,
	GRASS_LAU = 9,
	GRASS_LAU_2 = 10,
	GRASS_SAN_HO = 11
};
#pragma endregion [Define Terrain]
//////////////////////////////////////////////////////////////////////////
#pragma region [Define Character]
#define NUM_ANIMS 13           // number of animations the character has
#define CHAR_HEIGHT 5          // height of character's center of mass above ground
#define CAM_HEIGHT 2           // height of camera above character's center of mass
#define RUN_SPEED 100           // character running speed in units per second
#define TURN_SPEED 500.0f      // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second
#define JUMP_ACCEL 60.0f       // character jump acceleration in upward units per squared second
#define GRAVITY 90.0f          // gravity in downward units per squared second

// all the animations our character has, and a null ID
// some of these affect separate body parts and will be blended together
enum AnimID
{
	ANIM_IDLE_BASE,
	ANIM_IDLE_TOP,
	ANIM_RUN_BASE,
	ANIM_RUN_TOP,
	ANIM_HANDS_CLOSED,
	ANIM_HANDS_RELAXED,
	ANIM_DRAW_SWORDS,
	ANIM_SLICE_VERTICAL,
	ANIM_SLICE_HORIZONTAL,
	ANIM_DANCE,
	ANIM_JUMP_START,
	ANIM_JUMP_LOOP,
	ANIM_JUMP_END,
	ANIM_NONE
};
#pragma endregion [Define Character]
//////////////////////////////////////////////////////////////////////////
#endif