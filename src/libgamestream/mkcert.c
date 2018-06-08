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

#include "mkcert.h"

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/rsa.h>

#define RSA_KEY_SIZE 2048

void mkcert(const char* cert_file, const char* p12_file, const char* key_file)
{
	RsaKey key;
	WC_RNG rng;
	int ret;
	
	ret = wc_InitRng(&rng);
	if (ret != 0)
	{
		printf("[ERROR] Unable to init RNG : %d\n", ret);
		return;
	}
	
	ret = wc_InitRsaKey(&key, NULL);
	if (ret != 0)
	{
		printf("[ERROR] Unable to init RSA key : %d\n", ret);
		return;
	}
	
	ret = wc_MakeRsaKey(&key, RSA_KEY_SIZE, 65537, &rng);
    if(ret != 0) {
        printf("[ERROR] Unable to generate RSA key : %d\n", ret);
    }
	
	// TODO Generate a certificate + storage file and save everything
	
	wc_FreeRsaKey(&key);
	wc_FreeRng(&rng);
	
}