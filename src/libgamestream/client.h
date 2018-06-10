/*
    moonrise - Moonlight implementation for the Nintendo Switch
    Copyright (C) 2018  natinusala
	Copyright (C) 2015-2017 Iwan Timmer

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

#pragma once

#include "xml.h"

#include <stdbool.h>

#include <Limelight.h>

typedef struct _SERVER_DATA {
  const char* address;
  char* gpuType;
  bool paired;
  bool supports4K;
  bool unsupported;
  int currentGame;
  int serverMajorVersion;
  char* gsVersion;
  PDISPLAY_MODE modes;
  SERVER_INFORMATION serverInfo;
} SERVER_DATA, *PSERVER_DATA;

int gs_init(PSERVER_DATA server, char* address, const char* key_dir, bool unsupported);