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

#define GS_OK 0
#define GS_FAILED -1
#define GS_OUT_OF_MEMORY -2
#define GS_INVALID -3
#define GS_WRONG_STATE -4
#define GS_IO_ERROR -5
#define GS_NOT_SUPPORTED_4K -6
#define GS_UNSUPPORTED_VERSION -7
#define GS_NOT_SUPPORTED_MODE -8
#define GS_ERROR -9

extern const char* gs_error;
