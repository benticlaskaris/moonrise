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

#include <wolfssl/openssl/sha.h>
#include <wolfssl/openssl/aes.h>
#include <wolfssl/openssl/rand.h>
#include <wolfssl/openssl/evp.h>
#include <wolfssl/openssl/x509.h>
#include <wolfssl/openssl/pem.h>
#include <wolfssl/openssl/err.h>

#define UNIQUE_FILE_NAME "uniqueid.dat"
#define P12_FILE_NAME "client.p12"

#define UNIQUEID_BYTES 8
#define UNIQUEID_CHARS (UNIQUEID_BYTES*2)

static char unique_id[UNIQUEID_CHARS+1];

static WOLFSSL_X509* cert;
static char cert_hex[4096];
static WOLFSSL_EVP_PKEY* private_key;

const char* gs_error;

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

static int load_cert(const char* keyDirectory) {
  char certificateFilePath[PATH_MAX];
  snprintf(certificateFilePath, PATH_MAX, "%s/%s", keyDirectory, CERTIFICATE_FILE_NAME);

  char keyFilePath[PATH_MAX];
  snprintf(&keyFilePath[0], PATH_MAX, "%s/%s", keyDirectory, KEY_FILE_NAME);

  FILE *fd = fopen(certificateFilePath, "r");
  if (fd == NULL) {

    char p12FilePath[PATH_MAX];
    snprintf(p12FilePath, PATH_MAX, "%s/%s", keyDirectory, P12_FILE_NAME);
	
	printf("[INFO] Generating certificate...");
    mkcert(certificateFilePath, p12FilePath, keyFilePath);
    printf(" done\n");
    
    fd = fopen(certificateFilePath, "r");
  }

  if (fd == NULL) {
    gs_error = "Can't open certificate file";
    return GS_FAILED;
  }

  if (!(cert = PEM_read_X509(fd, NULL, NULL, NULL))) {
    gs_error = "Error loading cert into memory";
    return GS_FAILED;
  }

  rewind(fd);

  int c;
  int length = 0;
  while ((c = fgetc(fd)) != EOF) {
    sprintf(cert_hex + length, "%02x", c);
    length += 2;
  }
  cert_hex[length] = 0;

  fclose(fd);

  fd = fopen(keyFilePath, "r");
  if (fd == NULL) {
    gs_error = "Error loading key into memory";
    return GS_FAILED;
  }

  PEM_read_PrivateKey(fd, &private_key, NULL, NULL);
  fclose(fd);

  return GS_OK;
}

int gs_init(void* server, char* address, const char* key_dir, bool unsupported)
{
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

    return GS_OK;
}