/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "walt.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//walt character structure
enum
{
	walt_ArcMain_Idle0,
	walt_ArcMain_Idle1,
	walt_ArcMain_Idle2,
	walt_ArcMain_Idle3,
	walt_ArcMain_Idle4,
	walt_ArcMain_Left0,
	walt_ArcMain_Left1,
	walt_ArcMain_Left2,
	walt_ArcMain_Left3,
	walt_ArcMain_Down0,
	walt_ArcMain_Down1,
	walt_ArcMain_Down2,
	walt_ArcMain_Down3,
	walt_ArcMain_Up0,
	walt_ArcMain_Up1,
	walt_ArcMain_Up2,
	walt_ArcMain_Up3,
	walt_ArcMain_Right0,
	walt_ArcMain_Right1,
	walt_ArcMain_Right2,
	walt_ArcMain_Right3,
	
	walt_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[walt_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_walt;

//walt character definitions
static const CharFrame char_walt_frame[] = {
	{walt_ArcMain_Idle0, {  0,   0, 172, 194}, {100 - 40, 194}},
	{walt_ArcMain_Idle1, {  0,   0, 172, 196}, {100 - 40, 196}},
	{walt_ArcMain_Idle2, {  0,   0, 172, 197}, {100 - 40, 197}},
	{walt_ArcMain_Idle3, {  0,   0, 168, 195}, { 98 - 40, 195}},
	{walt_ArcMain_Idle4, {  0,   0, 172, 195}, { 99 - 40, 194}},

	{walt_ArcMain_Left0, {  0,   0, 172, 193}, {100 - 40, 193}},
	{walt_ArcMain_Left1, {  0,   0, 172, 194}, {100 - 40, 194}},
	{walt_ArcMain_Left2, {  0,   0, 172, 195}, {100 - 40, 195}},
	{walt_ArcMain_Left3, {  0,   0, 172, 195}, {100 - 40, 195}},

	{walt_ArcMain_Down0, {  0,   0, 176, 181}, {100 - 40, 181}},
	{walt_ArcMain_Down1, {  0,   0, 176, 187}, { 95 - 40, 187}},
	{walt_ArcMain_Down2, {  0,   0, 172, 192}, { 98 - 40, 192}},
	{walt_ArcMain_Down3, {  0,   0, 168, 195}, { 92 - 40, 195}},

	{walt_ArcMain_Up0, {  0,   0, 164, 202}, {100 - 40, 202}},
	{walt_ArcMain_Up1, {  0,   0, 164, 199}, {100 - 40, 199}},
	{walt_ArcMain_Up2, {  0,   0, 168, 197}, {100 - 40, 197}},
	{walt_ArcMain_Up3, {  0,   0, 172, 195}, {100 - 40, 195}},

	{walt_ArcMain_Right0, {  0,   0, 172, 192}, {100 - 40, 192}},
	{walt_ArcMain_Right1, {  0,   0, 172, 194}, {100 - 40, 194}},
	{walt_ArcMain_Right2, {  0,   0, 172, 195}, {100 - 40, 195}},
	{walt_ArcMain_Right3, {  0,   0, 172, 195}, {100 - 40, 195}},
};

static const Animation char_walt_anim[CharAnim_Max] = {
	{2, (const u8[]){0, 1, 2, 3,  ASCR_BACK, 0}}, //CharAnim_Idle
	{2, (const u8[]){5, 6, 7, 8, ASCR_BACK, 0}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){9, 10, ASCR_BACK, 0}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){13, 14, 15, 16, ASCR_BACK, 0}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){17, 18, 19, 20, ASCR_BACK, 0}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//walt character functions
void Char_walt_SetFrame(void *user, u8 frame)
{
	Char_walt *this = (Char_walt*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_walt_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_walt_Tick(Character *character)
{
	Char_walt *this = (Char_walt*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_walt_SetFrame);
	Character_Draw(character, &this->tex, &char_walt_frame[this->frame]);
}

void Char_walt_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_walt_Free(Character *character)
{
	Char_walt *this = (Char_walt*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_walt_New(fixed_t x, fixed_t y)
{
	//Allocate walt object
	Char_walt *this = Mem_Alloc(sizeof(Char_walt));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_walt_New] Failed to allocate walt object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_walt_Tick;
	this->character.set_anim = Char_walt_SetAnim;
	this->character.free = Char_walt_Free;
	
	Animatable_Init(&this->character.animatable, char_walt_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;

	//health bar color
	this->character.health_bar = 0xFFAD63D6;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\WALT.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //walt_ArcMain_Idle0
		"idle1.tim", //walt_ArcMain_Idle1
		"idle2.tim", //walt_ArcMain_Idle1
		"idle3.tim", //walt_ArcMain_Idle1
		"idle4.tim", //walt_ArcMain_Idle1
		"left0.tim",  //walt_ArcMain_Left
		"left1.tim",  //walt_ArcMain_Left
		"left2.tim",  //walt_ArcMain_Left
		"left3.tim",  //walt_ArcMain_Left
		"down0.tim",  //walt_ArcMain_Down
		"down1.tim",  //walt_ArcMain_Down
		"down2.tim",  //walt_ArcMain_Down
		"down3.tim",  //walt_ArcMain_Down
		"up0.tim",    //walt_ArcMain_Up
		"up1.tim",    //walt_ArcMain_Up
		"up2.tim",    //walt_ArcMain_Up
		"up3.tim",    //walt_ArcMain_Up
		"right0.tim", //walt_ArcMain_Right
		"right1.tim", //walt_ArcMain_Right
		"right2.tim", //walt_ArcMain_Right
		"right3.tim", //walt_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
