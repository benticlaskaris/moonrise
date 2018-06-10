/*
    moonrise - Moonlight implementation for the Nintendo Switch
    Copyright (C) 2018  natinusala

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <stdio.h>

#include <switch.h>

#include "libgamestream/client.h"
#include "libgamestream/errors.h"

#define MAX_ADDRESS_SIZE 40

#define MOONRISE_VERSION "0.1"

#define KEY_DIR "/switch/moonrise"

struct {
	char* address;				// server address
	bool unsupported;			// accept unsupported video modes ?
} config;

int main(int argc, char **argv)
{
	gfxInitDefault();
	csrngInitialize();
	socketInitializeDefault();
	
	consoleInit(NULL);
	
	printf("----------------------------------------------\n");
	printf("Moonrise v" MOONRISE_VERSION " by natinusala\n");
	printf("Adapted from Moonlight Embedded by Iwan Timmer\n");
	printf("----------------------------------------------\n\n\n");
	
	// TODO Read the config from a file, with different defaults
	config.address = "192.168.100.139";
	config.unsupported = false;
	
	printf("[INFO] Initialization...\n");
	
	int ret;
	
	if ((ret = gs_init(NULL, config.address, KEY_DIR, config.unsupported)) == GS_OUT_OF_MEMORY) 
	{
		printf("[ERROR] Not enough memory\n");
	}
	else if (ret == GS_ERROR)
	{
		printf("[ERROR] GameStream error %s\n", gs_error);
	}
	else if (ret == GS_INVALID) 
	{
		printf("[ERROR] Invalid data received from server %s\n", gs_error);
	} 
	else if (ret == GS_UNSUPPORTED_VERSION) 
	{
		printf("[ERROR] Unsupported version %s\n", gs_error);
	} 
	else if (ret != GS_OK) 
	{
		printf("[ERROR] Initialization failed\n");
	}
	
	while (appletMainLoop())
	{
		hidScanInput();
		
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		
		if (kDown & KEY_PLUS) break;
		
		gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
	}
	
	gfxExit();
	socketExit();
	csrngExit();
    return 0;
}