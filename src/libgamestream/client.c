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

#include "client.h"
#include "http.h"
#include "errors.h"
#include "mkcert.h"

#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <switch.h>
#include <stdio.h>

#include <wolfssl/ssl.h>
#include <wolfssl/openssl/ssl.h>
#include <wolfssl/wolfcrypt/asn_public.h>

#include <uuid/uuid.h>

#define UNIQUE_FILE_NAME "uniqueid.dat"
#define P12_FILE_NAME "client.p12"

#define UNIQUEID_BYTES 8
#define UNIQUEID_CHARS (UNIQUEID_BYTES*2)

static char unique_id[UNIQUEID_CHARS+1];

static WOLFSSL_X509* cert;
static char cert_hex[4096];

static WOLFSSL_EVP_PKEY* private_key;

const char* gs_error;

static int load_server_status(PSERVER_DATA server)
{
	//TODO
	return 0;
}

static int load_unique_id(const char* keyDirectory) {
    char uniqueFilePath[PATH_MAX];
    snprintf(uniqueFilePath, PATH_MAX, "%s/%s", keyDirectory, UNIQUE_FILE_NAME);

    FILE *fd = fopen(uniqueFilePath, "r");
    if (fd == NULL) {
        printf("[INFO] Generating new Unique ID\n");
        unsigned char unique_data[UNIQUEID_BYTES];
        randomGet(unique_data, UNIQUEID_BYTES);
        for (int i = 0; i < UNIQUEID_BYTES; i++) {
            sprintf(unique_id + (i * 2), "%02x", unique_data[i]);
        }
        fd = fopen(uniqueFilePath, "w");
        if (fd == NULL)
            return GS_FAILED;

        fwrite(unique_id, UNIQUEID_CHARS, 1, fd);
    } else {
        fread(unique_id, UNIQUEID_CHARS, 1, fd);
    }
    fclose(fd);
    unique_id[UNIQUEID_CHARS] = 0;

    return GS_OK;
}

static int load_cert(const char* keyDirectory) 
{
  wolfSSL_add_all_algorithms();
  
  char certificateFilePath[PATH_MAX];
  snprintf(certificateFilePath, PATH_MAX, "%s/%s", keyDirectory, CERTIFICATE_FILE_NAME);

  char keyFilePath[PATH_MAX];
  snprintf(&keyFilePath[0], PATH_MAX, "%s/%s", keyDirectory, KEY_FILE_NAME);
  
  char p12FilePath[PATH_MAX];
  snprintf(p12FilePath, PATH_MAX, "%s/%s", keyDirectory, P12_FILE_NAME);

  FILE *fd = fopen(certificateFilePath, "r");
  if (fd == NULL) 
  {
	printf("[INFO] Generating certificate...\n");
    mkcert(certificateFilePath, p12FilePath, keyFilePath);
    printf("[INFO] Done generating certificare\n");
    
    fd = fopen(certificateFilePath, "r");
  }
  
  //Open certificate

  if (fd == NULL) {
    gs_error = "Can't open certificate file";
    return GS_FAILED;
  }
  
  struct stat stat_buf;
  
  stat(certificateFilePath, &stat_buf);
  
  byte* cert_buf = malloc(stat_buf.st_size);
  
  fread(cert_buf, stat_buf.st_size, 1, fd);
  
  byte der_cert[4096];
  
  int der_cert_size = wc_CertPemToDer(cert_buf, stat_buf.st_size, der_cert, sizeof(der_cert), CERT_TYPE);
  
  if (der_cert_size < 0)
  {
	  gs_error = "Cannot load certificate";
	  printf("[ERROR] Cannot convert certificate to DER : %d\n", der_cert_size);
	  return GS_FAILED;
  }
  
  const byte* tmp = der_cert;
      
  cert = wolfSSL_d2i_X509(NULL, &tmp, der_cert_size);
  
  if (cert == NULL)
  {
	  gs_error = "Cannot load certificate into memory";
	  return GS_FAILED;
  }
  
  free(cert_buf);

  //Generate certificate hex

  rewind(fd);

  int c;
  int length = 0;
  while ((c = fgetc(fd)) != EOF) {
    sprintf(cert_hex + length, "%02x", c);
    length += 2;
  }
  cert_hex[length] = 0;

  fclose(fd);
  
  printf("[INFO] Certificate loaded\n");
  
  //Load private key
  
  printf("[INFO] Loading private key...\n");

  fd = fopen(keyFilePath, "r");
  if (fd == NULL) {
    gs_error = "Can't open key file";
    return GS_FAILED;
  }
  
  stat(keyFilePath, &stat_buf);
  
  byte* key_buf = malloc(stat_buf.st_size);
  
  fread(key_buf, stat_buf.st_size, 1, fd);

  fclose(fd);
  
  byte der_key[4096];
  
  int der_key_size = wc_KeyPemToDer(key_buf, stat_buf.st_size, der_key, sizeof(der_key), NULL);
  
  if (der_key_size < 0)
  {
	  gs_error = "Cannot convert key to DER";
	  printf("[ERROR] Cannot convert key to DER : %d\n", der_key_size);
	  return GS_FAILED;
  }
  
  const byte* tmp2 = der_key;
      
  private_key = wolfSSL_d2i_PrivateKey(EVP_PKEY_RSA, NULL, &tmp2, der_key_size);
  
  if (private_key == NULL)
  {
	  gs_error = "Cannot load key into memory";
	  return GS_FAILED;
  }
  
  free(key_buf);
  
  return GS_OK;
}

int gs_init(PSERVER_DATA server, char* address, const char* key_dir, bool unsupported)
{
	mkdir(key_dir, 777);
	
    if (load_unique_id(key_dir) != GS_OK)
    {
        printf("[ERROR] Unable to load Unique ID\n");
        return GS_FAILED;
    }

    if (load_cert(key_dir) != GS_OK)
    {
        printf("[ERROR] Unable to load certificate : %s\n", gs_error);
        return GS_FAILED;
    }

    printf("[INFO] Unique ID : %s\n", unique_id);
	
	  
	printf("[INFO] Initialization done\n");
	  
	//Connection
	
	if (http_init(key_dir) != GS_OK)
	{
		printf("[ERROR] Unable to init HTTP engine\n");
        return GS_FAILED;
	}
	  
	printf("[INFO] Connecting to %s...\n", address);
	
	//TODO This :
	
	/*LiInitializeServerInformation(&server->serverInfo);
	server->serverInfo.address = address;
	server->unsupported = unsupported;
	
    return load_server_status(server);*/
	
	return 0;
}