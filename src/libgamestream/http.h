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

#define CERTIFICATE_FILE_NAME "client.pem"
#define KEY_FILE_NAME "key.pem"

#include <stdio.h>

typedef struct _HTTP_DATA {
  char *memory;
  size_t size;
} HTTP_DATA, *PHTTP_DATA;

int http_init(const char* key_dir);
PHTTP_DATA http_create_data();
int http_request(char* url, PHTTP_DATA data);
void http_free_data(PHTTP_DATA data);