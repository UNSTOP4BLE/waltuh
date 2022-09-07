/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "jesse.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//jesse character structure
enum
{
	jesse_ArcMain_Idle0,
	jesse_ArcMain_Idle1,
	jesse_ArcMain_Idle2,
	jesse_ArcMain_Idle3,
	jesse_ArcMain_Idle4,
	jesse_ArcMain_Left0,
	jesse_ArcMain_Left1,
	jesse_ArcMain_Left2,
	jesse_ArcMain_Left3,
	jesse_ArcMain_Down0,
	jesse_ArcMain_Down1,
	jesse_ArcMain_Down2,
	jesse_ArcMain_Down3,
	jesse_ArcMain_Up0,
	jesse_ArcMain_Up1,
	jesse_ArcMain_Up2,
	jesse_ArcMain_Up3,
	jesse_ArcMain_Right0,
	jesse_ArcMain_Right1,
	jesse_ArcMain_Right2,
	jesse_ArcMain_Right3,
	
	jesse_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[jesse_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_jesse;

//jesse character definitions
static const CharFrame char_jesse_frame[] = {
	{jesse_ArcMain_Idle0, {  0,   0, 172, 194}, {100 - 40, 194}},
	{jesse_ArcMain_Idle1, {  0,   0, 172, 196}, {100 - 40, 196}},
	{jesse_ArcMain_Idle2, {  0,   0, 172, 197}, {100 - 40, 197}},
	{jesse_ArcMain_Idle3, {  0,   0, 168, 195}, { 98 - 40, 195}},
	{jesse_ArcMain_Idle4, {  0,   0, 172, 195}, { 99 - 40, 194}},

	{jesse_ArcMain_Left0, {  0,   0, 172, 193}, {100 - 40, 193}},
	{jesse_ArcMain_Left1, {  0,   0, 172, 194}, {100 - 40, 194}},
	{jesse_ArcMain_Left2, {  0,   0, 172, 195}, {100 - 40, 195}},
	{jesse_ArcMain_Left3, {  0,   0, 172, 195}, {100 - 40, 195}},

	{jesse_ArcMain_Down0, {  0,   0, 176, 181}, {100 - 40, 181}},
	{jesse_ArcMain_Down1, {  0,   0, 176, 187}, { 99 - 40, 188}},
	{jesse_ArcMain_Down2, {  0,   0, 172, 192}, { 98 - 40, 193}},
	{jesse_ArcMain_Down3, {  0,   0, 168, 195}, { 99 - 40, 195}},

	{jesse_ArcMain_Up0, {  0,   0, 164, 202}, {100 - 40, 202}},
	{jesse_ArcMain_Up1, {  0,   0, 164, 199}, {102 - 40, 199}},
	{jesse_ArcMain_Up2, {  0,   0, 168, 197}, {102 - 40, 197}},
	{jesse_ArcMain_Up3, {  0,   0, 172, 195}, {103 - 40, 195}},

	{jesse_ArcMain_Right0, {  0,   0, 172, 192}, {100 - 40, 192}},
	{jesse_ArcMain_Right1, {  0,   0, 172, 194}, {100 - 40, 194}},
	{jesse_ArcMain_Right2, {  0,   0, 172, 195}, {100 - 40, 195}},
	{jesse_ArcMain_Right3, {  0,   0, 172, 195}, {100 - 40, 195}},
};

static const Animation char_jesse_anim[CharAnim_Max] = {
	{2, (const u8[]){0, 1, 2, 3,  ASCR_BACK, 0}}, //CharAnim_Idle
	{2, (const u8[]){5, 6, 7, 8, ASCR_BACK, 0}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){9, 10, 11, 12, ASCR_BACK, 0}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){13, 14, 15, 16, ASCR_BACK, 0}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){17, 18, 19, 20, ASCR_BACK, 0}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//jesse character functions
void Char_jesse_SetFrame(void *user, u8 frame)
{
	Char_jesse *this = (Char_jesse*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_jesse_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_jesse_Tick(Character *character)
{
	Char_jesse *this = (Char_jesse*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_jesse_SetFrame);
	Character_Draw(character, &this->tex, &char_jesse_frame[this->frame]);
}

void Char_jesse_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_jesse_Free(Character *character)
{
	Char_jesse *this = (Char_jesse*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_jesse_New(fixed_t x, fixed_t y)
{
	//Allocate jesse object
	Char_jesse *this = Mem_Alloc(sizeof(Char_jesse));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_jesse_New] Failed to allocate jesse object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_jesse_Tick;
	this->character.set_anim = Char_jesse_SetAnim;
	this->character.free = Char_jesse_Free;
	
	Animatable_Init(&this->character.animatable, char_jesse_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFAD63D6;
	
	this->character.focus_x = FIXED_DEC(-90 - -120,1);
	this->character.focus_y = FIXED_DEC(-14 - 100,1);
	this->character.focus_zoom = FIXED_DEC(14,10);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\JESSE.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //jesse_ArcMain_Idle0
		"idle1.tim", //jesse_ArcMain_Idle1
		"idle2.tim", //jesse_ArcMain_Idle1
		"idle3.tim", //jesse_ArcMain_Idle1
		"idle4.tim", //jesse_ArcMain_Idle1
		"left0.tim",  //jesse_ArcMain_Left
		"left1.tim",  //jesse_ArcMain_Left
		"left2.tim",  //jesse_ArcMain_Left
		"left3.tim",  //jesse_ArcMain_Left
		"down0.tim",  //jesse_ArcMain_Down
		"down1.tim",  //jesse_ArcMain_Down
		"down2.tim",  //jesse_ArcMain_Down
		"down3.tim",  //jesse_ArcMain_Down
		"up0.tim",    //jesse_ArcMain_Up
		"up1.tim",    //jesse_ArcMain_Up
		"up2.tim",    //jesse_ArcMain_Up
		"up3.tim",    //jesse_ArcMain_Up
		"right0.tim", //jesse_ArcMain_Right
		"right1.tim", //jesse_ArcMain_Right
		"right2.tim", //jesse_ArcMain_Right
		"right3.tim", //jesse_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
