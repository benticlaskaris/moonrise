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
#include <wolfssl/wolfcrypt/asn_public.h>

#define RSA_KEY_SIZE 2048
#define VALIDITY_YEARS 10

void mkcert(const char* cert_file, const char* p12_file, const char* key_file)
{
	RsaKey key;
	WC_RNG rng;
	int ret;
	
	//Init RNG
	
	ret = wc_InitRng(&rng);
	if (ret != 0)
	{
		printf("[ERROR] Unable to init RNG : %d\n", ret);
		return;
	}
	
	//Init RSA
	
	ret = wc_InitRsaKey(&key, NULL);
	if (ret != 0)
	{
		printf("[ERROR] Unable to init RSA key : %d\n", ret);
		return;
	}
	
	//Make RSA
	
	ret = wc_MakeRsaKey(&key, RSA_KEY_SIZE, 65537, &rng);
    if(ret != 0) {
        printf("[ERROR] Unable to generate RSA key : %d\n", ret);
    }
	
	//Save RSA
	
	//TODO
	
	//Make cert
	
	Cert cert;
	
	wc_InitCert(&cert);
	
	strncpy(cert.subject.commonName, "NVIDIA GameStream Client", CTC_NAME_SIZE);
	strncpy(cert.issuer.commonName, "NVIDIA GameStream Client", CTC_NAME_SIZE);
	
	cert.daysValid = VALIDITY_YEARS * 365;
	
	cert.serial[0] = 0;
	cert.serialSz = 1;
	
	cert.isCA = 0;
    cert.sigType = CTC_SHAwRSA;
	
	byte der_cert[4096];
	
	int der_size = wc_MakeSelfCert(&cert, der_cert, sizeof(der_cert), &key, &rng);
	
	if (der_size < 0)
	{
		printf("[ERROR] Unable to generate certificate : %d\n", ret);
		return;
	}
	
	//Save cert
	
	byte pem_cert[4096];
	
	int pem_size = wc_DerToPem(der_cert, der_size, pem_cert, sizeof(pem_cert), CERT_TYPE);
	
	if (pem_size < 0)
	{
		printf("[ERROR] Unable to convert certificate : %d\n", ret);
		return;
	}
	
	FILE* fd = fopen(cert_file, "w");
	
	fwrite(pem_cert, pem_size, 1, fd);
	
	fclose(fd);
	
	// TODO storage file
	
	//Cleanup
	
	wc_FreeRsaKey(&key);
	wc_FreeRng(&rng);	
}