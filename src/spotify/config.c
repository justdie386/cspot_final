#include <spotify/config.h>
#include <spotify/spotify.h>
#include <cjson/cJSON.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

char *spotify_new_config_path(char *format, ...)
{
  va_list args;

  va_start(args, format);
  int size = vsnprintf(NULL, 0, format, args);
  va_end(args);

  char *format_char = (char *)malloc(size + 1);

  va_start(args, format);
  vsnprintf(format_char, size + 1, format, args);
  va_end(args);

  return format_char;
}

int path_exist(const char *target)
{
  struct stat buffer;
  return stat(target, &buffer) == 0 ? 0 : 1;
}

int spotify_config_exist(const char *filename)
{
  return path_exist(filename);
}

void spotify_config_create(const char *folder, const char *filename, const char *refresh_token, const char *encoded_id)
{
  if (path_exist(folder) == 1)
  {
    printf("[+] Creating folder path: %s\n", folder);
    mkdir(folder, 0755);
  }

  cJSON *config = cJSON_CreateObject();
  cJSON_AddStringToObject(config, "refresh_token", refresh_token);
  cJSON_AddStringToObject(config, "encoded_id", encoded_id);

  char *config_data = cJSON_Print(config);

  FILE *config_file = fopen(filename, "w");
  fputs(config_data, config_file);
  fclose(config_file);
  cJSON_free(config_data);
  cJSON_Delete(config);
}

void spotify_config_open(spotify_login *login, const char *filename)
{
  FILE *file = fopen(filename, "rb");

  fseek(file, 0L, SEEK_END);
  size_t file_size = ftell(file);
  fseek(file, 0L, SEEK_SET);

  // Allocate memory for file content
  char *file_content = (char *)malloc(file_size + 1);

  fread(file_content, 1, file_size, file);
  file_content[file_size] = '\0';

  cJSON *config = cJSON_Parse(file_content);

  char *refresh_token = cJSON_GetObjectItemCaseSensitive(config, "refresh_token")->valuestring;
  char *encoded_id = cJSON_GetObjectItemCaseSensitive(config, "encoded_id")->valuestring;
  
  login->refresh_token = (char *)malloc(strlen(refresh_token) + 1);
  strcpy(login->refresh_token, refresh_token);
  login->encoded_id = (char *)malloc(strlen(encoded_id) +1);
  strcpy(login->encoded_id, encoded_id);
  login->access_token = NULL;
  login->authorization_token = NULL;
  cJSON_Delete(config);
}