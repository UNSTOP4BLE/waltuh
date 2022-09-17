/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bf.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

int secret;

//Boyfriend player types
enum
{
	BF_ArcMain_Idle0,
	BF_ArcMain_Idle1,
	BF_ArcMain_Idle2,
	BF_ArcMain_Idle3,

	BF_ArcMain_Left0,
	BF_ArcMain_Left1,
	BF_ArcMain_Left2,
	BF_ArcMain_Left3,

	BF_ArcMain_Down0,
	BF_ArcMain_Down1,
	BF_ArcMain_Down2,
	BF_ArcMain_Down3,
	
	BF_ArcMain_Up0,
	BF_ArcMain_Up1,
	BF_ArcMain_Up2,
	BF_ArcMain_Up3,

	BF_ArcMain_Right0,
	BF_ArcMain_Right1,
	BF_ArcMain_Right2,
	BF_ArcMain_Right3,

	BF_ArcMain_Leftm0,
	BF_ArcMain_Leftm1,

	BF_ArcMain_Downm0,
	BF_ArcMain_Downm1,
	
	BF_ArcMain_Upm0,
	BF_ArcMain_Upm1,

	BF_ArcMain_Rightm0,
	BF_ArcMain_Rightm1,
	
	BF_ArcMain_Max,
};

#define BF_Arc_Max BF_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[BF_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
	
} Char_BF;

//Boyfriend player definitions
static const CharFrame char_bf_frame[] = {
	{BF_ArcMain_Idle0, {  0,   0, 144, 194}, {  0, 194}}, 
	{BF_ArcMain_Idle1, {  0,   0, 140, 198}, {  0, 198}}, 
	{BF_ArcMain_Idle2, {  0,   0, 140, 200}, {  0, 200}}, 
	{BF_ArcMain_Idle3, {  0,   0, 140, 198}, {  0, 198}}, 

	{BF_ArcMain_Left0, {  0,   0, 136, 198}, {  0, 198}}, 
	{BF_ArcMain_Left1, {  0,   0, 140, 198}, {  0, 198}}, 
	{BF_ArcMain_Left2, {  0,   0, 136, 198}, {  0, 198}}, 
	{BF_ArcMain_Left3, {  0,   0, 140, 198}, {  0, 198}}, 

	{BF_ArcMain_Down0, {  0,   0, 144, 193}, {  0, 193}}, 
	{BF_ArcMain_Down1, {  0,   0, 144, 196}, {  0, 196}}, 
	{BF_ArcMain_Down2, {  0,   0, 140, 198}, {  0, 198}}, 
	{BF_ArcMain_Down3, {  0,   0, 136, 199}, {  0, 199}}, 

	{BF_ArcMain_Up0, {  0,   0, 132, 207}, {  0, 207}}, 
	{BF_ArcMain_Up1, {  0,   0, 136, 203}, {  0, 203}},
	{BF_ArcMain_Up2, {  0,   0, 136, 201}, {  0, 201}}, 
	{BF_ArcMain_Up3, {  0,   0, 136, 199}, {  0, 199}}, 

	{BF_ArcMain_Right0, {  0,   0, 140, 198}, {  0, 198}}, 
	{BF_ArcMain_Right1, {  0,   0, 140, 198}, {  0, 198}}, 
	{BF_ArcMain_Right2, {  0,   0, 140, 198}, {  0, 198}}, 
	{BF_ArcMain_Right3, {  0,   0, 140, 198}, {  0, 198}}, 
	
	{BF_ArcMain_Leftm0, {  0,   0, 140, 198}, {  0, 198}}, 
	{BF_ArcMain_Leftm1, {  0,   0, 140, 198}, {  0, 198}}, 

	{BF_ArcMain_Downm0, {  0,   0, 140, 199}, {  0, 199}}, 
	{BF_ArcMain_Downm1, {  0,   0, 140, 196}, {  0, 196}}, 

	{BF_ArcMain_Upm0, {  0,   0, 140, 199}, {  0, 199}}, 
	{BF_ArcMain_Upm1, {  0,   0, 132, 203}, {  0, 203}},

	{BF_ArcMain_Rightm0, {  0,   0, 140, 198}, {  0, 198}}, 
	{BF_ArcMain_Rightm1, {  0,   0, 140, 199}, {  0, 199}}, 
};

static const Animation char_bf_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  ASCR_BACK, 0}}, //CharAnim_Idle
	{2, (const u8[]){ 4,  5,  6,  7,  ASCR_BACK, 0}},             //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 8,  9, 10, 11,  ASCR_BACK, 0}},             //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_DownAlt
	{2, (const u8[]){12, 13, 14, 15,  ASCR_BACK, 0}},             //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_UpAlt
	{2, (const u8[]){16, 17, 18, 19, ASCR_BACK, 0}},             //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 4, 20, 20, 21, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 8, 22, 22, 23, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){12, 24, 24, 25, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){16, 26, 26, 27, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
};

//Boyfriend player functions
void Char_BF_SetFrame(void *user, u8 frame)
{
	Char_BF *this = (Char_BF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bf_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BF_Tick(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Secret icon
	if (secret)
		this->character.health_i = 3;
	else
		this->character.health_i = 0;

	if (pad_state.press & PAD_SELECT && stage.prefs.debug == 0)
		secret ++;
	
	if (secret == 2)
		secret = 0;
	
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
		
		
	}

	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BF_SetFrame);
	Character_Draw(character, &this->tex, &char_bf_frame[this->frame]);
}

void Char_BF_SetAnim(Character *character, u8 anim)
{
	Char_BF *this = (Char_BF*)character;
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BF_Free(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_BF_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_BF *this = Mem_Alloc(sizeof(Char_BF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BF_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BF_Tick;
	this->character.set_anim = Char_BF_SetAnim;
	this->character.free = Char_BF_Free;
	
	Animatable_Init(&this->character.animatable, char_bf_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;

	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(60,1);
	this->character.focus_y = FIXED_DEC(-12 - 100,1);
	this->character.focus_zoom = FIXED_DEC(14,10);
	
	//Load art
		this->arc_main = IO_Read("\\CHAR\\BF.ARC;1");
		
		const char **pathp = (const char *[]){
			"idle0.tim", 
			"idle1.tim", 
			"idle2.tim", 
			"idle3.tim", 
			
			"left0.tim", 
			"left1.tim", 
			"left2.tim", 
			"left3.tim", 

			"down0.tim",
			"down1.tim",
			"down2.tim",
			"down3.tim",

			"up0.tim",
			"up1.tim",
			"up2.tim",
			"up3.tim",

			"right0.tim",
			"right1.tim",
			"right2.tim",
			"right3.tim",
			
			"leftm0.tim",
			"leftm1.tim",
			
			"downm0.tim",
			"downm1.tim",
			
			"upm0.tim",
			"upm1.tim",
			
			"rightm0.tim",
			"rightm1.tim",
			NULL
		};
		IO_Data *arc_ptr = this->arc_ptr;
		for (; *pathp != NULL; pathp++)
			*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	
	return (Character*)this;
}
