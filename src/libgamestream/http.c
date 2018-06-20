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

#include "http.h"
#include "errors.h"

#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define HTTP_DEBUG true

static CURL *curl;

static size_t _write_curl(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  PHTTP_DATA mem = (PHTTP_DATA)userp;
 
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL)
    return 0;
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

int http_request(char* url, PHTTP_DATA data) {
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
  curl_easy_setopt(curl, CURLOPT_URL, url);

  if (HTTP_DEBUG)
    printf("[DEBUG] HTTP Request %s\n", url);

  if (data->size > 0) {
    free(data->memory);
    data->memory = malloc(1);
    if(data->memory == NULL)
      return GS_OUT_OF_MEMORY;

    data->size = 0;
  }
  CURLcode res = curl_easy_perform(curl);
  
  if(res != CURLE_OK) {
    gs_error = curl_easy_strerror(res);
    return GS_FAILED;
  } else if (data->memory == NULL) {
    return GS_OUT_OF_MEMORY;
  }

  if (HTTP_DEBUG)
    printf("[DEBUG] HTTP Response:\n%s\n\n", data->memory);
  
  return GS_OK;
}

int http_init(const char* key_dir)
{
	curl = curl_easy_init();
	
	if (!curl)
		return GS_FAILED;
	
	char certificateFilePath[4096];
	sprintf(certificateFilePath, "%s/%s", key_dir, CERTIFICATE_FILE_NAME);

	char keyFilePath[4096];
	sprintf(&keyFilePath[0], "%s/%s", key_dir, KEY_FILE_NAME);

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT, 1L);
	curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE,"PEM");
	curl_easy_setopt(curl, CURLOPT_SSLCERT, certificateFilePath);
	curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
	curl_easy_setopt(curl, CURLOPT_SSLKEY, keyFilePath);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_curl);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
	curl_easy_setopt(curl, CURLOPT_SSL_SESSIONID_CACHE, 0L);
    
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_STDERR, stdout);
	
	return GS_OK;
}

PHTTP_DATA http_create_data() {
  PHTTP_DATA data = malloc(sizeof(HTTP_DATA));
  if (data == NULL)
    return NULL;

  data->memory = malloc(1);
  if(data->memory == NULL) {
    free(data);
    return NULL;
  }
  data->size = 0;

  return data;
}

void http_free_data(PHTTP_DATA data) {
  if (data != NULL) {
    if (data->memory != NULL)
      free(data->memory);

    free(data);
  }
}