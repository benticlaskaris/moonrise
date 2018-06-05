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

int main(int argc, char **argv)
{
	gfxInitDefault();
	
	consoleInit(NULL);
	
	printf("The Moon rises on the Horizon...");
	
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
    return 0;
}