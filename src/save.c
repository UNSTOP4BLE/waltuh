/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

//thanks to spicyjpeg for helping me understand this code

#include "save.h"

#include <libmcrd.h>
#include "stage.h"
				  
	        //HAS to be BASCUS-scusid,somename
#define savetitle "bu00:BASCUS-00000walt"
#define savename  "Vs Walter"

static const u8 saveIconPalette[32] = 
{
  	0x9D, 0xEF, 0x2C, 0x9D, 0x8B, 0x88, 0x2E, 0x95, 0xC9, 0x98, 0x4D, 0xAD,
    0x13, 0xB6, 0xB9, 0xC6, 0x5E, 0xD3, 0xA8, 0x90, 0x91, 0xA9, 0x53, 0xA1,
    0xB6, 0xA9, 0x62, 0x98, 0x48, 0xB9, 0x00, 0x00
};

static const u8 saveIconImage[128] = 
{
    0x00, 0x10, 0x22, 0x22, 0x23, 0x54, 0x51, 0x06, 0x00, 0x50, 0x63, 0x65,
    0x87, 0xA9, 0xA1, 0x39, 0x45, 0x3A, 0x53, 0x5A, 0x85, 0x20, 0xA9, 0x99,
    0x99, 0x92, 0x5A, 0xBB, 0xBB, 0x70, 0x93, 0x11, 0x99, 0x32, 0xCA, 0xBB,
    0xBB, 0x8B, 0x56, 0x94, 0x51, 0x1B, 0xCC, 0xBC, 0xBB, 0x8B, 0x16, 0x94,
    0x16, 0xA2, 0xBC, 0xB2, 0xBB, 0x8B, 0x46, 0x44, 0xA6, 0x1A, 0xBC, 0xBB,
    0xBB, 0x8C, 0x16, 0x51, 0x76, 0x68, 0xCC, 0xBC, 0xCB, 0x8C, 0x3A, 0x11,
    0x76, 0x73, 0xCA, 0xBC, 0xBB, 0xAC, 0x13, 0x11, 0x89, 0x65, 0xCD, 0x2B,
    0x92, 0x8C, 0x31, 0x11, 0x66, 0x66, 0xBE, 0x22, 0xB2, 0x87, 0x88, 0x11,
    0x66, 0x66, 0x2E, 0x22, 0x22, 0x8E, 0x76, 0x08, 0x66, 0xA6, 0xDD, 0x22,
    0x34, 0x7D, 0x66, 0x66, 0x6A, 0xA6, 0xDD, 0x6E, 0xE5, 0x6A, 0x66, 0x66,
    0xAA, 0xAA, 0xEE, 0xDE, 0xDE, 0x8A, 0x66, 0x66
};

static void toShiftJIS(u8 *buffer, const char *text)
{
    int pos = 0;
    for (u32 i = 0; i < strlen(text); i++) 
    {
        u8 c = text[i];
        if (c >= '0' && c <= '9') { buffer[pos++] = 0x82; buffer[pos++] = 0x4F + c - '0'; }
        else if (c >= 'A' && c <= 'Z') { buffer[pos++] = 0x82; buffer[pos++] = 0x60 + c - 'A'; }
        else if (c >= 'a' && c <= 'z') { buffer[pos++] = 0x82; buffer[pos++] = 0x81 + c - 'a'; }
        else if (c == '(') { buffer[pos++] = 0x81; buffer[pos++] = 0x69; }
        else if (c == ')') { buffer[pos++] = 0x81; buffer[pos++] = 0x6A; }
        else /* space */ { buffer[pos++] = 0x81; buffer[pos++] = 0x40; }
    }
}

static void initSaveFile(SaveFile *file, const char *name) 
{
	file->id = 0x4353;
 	file->iconDisplayFlag = 0x11;
 	file->iconBlockNum = 1;
  	toShiftJIS(file->title, name);
 	memcpy(file->iconPalette, saveIconPalette, 32);
 	memcpy(file->iconImage, saveIconImage, 128);
}

void defaultSettings()
{
	stage.prefs.sfxmiss = 1;
	stage.prefs.botplay = 1;	
	stage.prefs.songtimer = 1;

	for (int i = 0; i < StageId_Max; i++)
	{
		stage.prefs.savescore[i][0] = 0;
		stage.prefs.savescore[i][1] = 0;
		stage.prefs.savescore[i][2] = 0;
	}
}

boolean readSaveFile()
{
	int fd = open(savetitle, 0x0001);
	if (fd < 0) // file doesnt exist 
		return false;

	SaveFile file;
	if (read(fd, (void *) &file, sizeof(SaveFile)) == sizeof(SaveFile)) 
		printf("ok\n");
	else {
		printf("read error\n");
		return false;
	}
	memcpy((void *) &stage.prefs, (const void *) file.saveData, sizeof(stage.prefs));
	close(fd);
	return true;
}

void writeSaveFile()
{	
	int fd = open(savetitle, 0x0002);

	if (fd < 0) // if save doesnt exist make one
		fd =  open(savetitle, 0x0202 | (1 << 16));

	SaveFile file;
	initSaveFile(&file, savename);
  	memcpy((void *) file.saveData, (const void *) &stage.prefs, sizeof(stage.prefs));
	
	if (fd >= 0) {
	  	if (write(fd, (void *) &file, sizeof(SaveFile)) == sizeof(SaveFile)) 
	  		printf("ok\n");
	 	else 
	 		printf("write error\n");  // if save doesnt exist do a error
		close(fd);
	} 
	else 
		printf("open error %d\n", fd);  // failed to save
}