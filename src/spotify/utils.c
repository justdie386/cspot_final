#include "spotify/base64.h"
#include <curl/curl.h>
#include <spotify/spotify.h>
#include <spotify/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// found this function online, not sure of how good, accurate, and so on it is,
// but it doesn't seem to cause issues

size_t writefunc(void *ptr, size_t size, size_t nmemb, char **str) {
  size_t new_len = (*str == NULL ? 0 : strlen(*str)) + size * nmemb;

  *str = (char *)realloc(*str, new_len + 1);
  if (*str == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }

  memcpy(*str + (new_len - size * nmemb), ptr, size * nmemb);
  (*str)[new_len] = '\0';

  return size * nmemb;
}

spotify_body *spotify_new_body()
{
  spotify_body *body = malloc(sizeof(*body));
  if (!body)
    return NULL;

  return body;
}

void spotify_body_add(spotify_body *body, char *item, char *format, ...)
{
  va_list args;

  va_start(args, format);
  int args_size = vsnprintf(NULL, 0, format, args);
  va_end(args);

  size_t total_size = args_size + strlen(item);
  char *format_char = (char *)malloc(args_size + 1);

  va_start(args, format);
  vsnprintf(format_char, args_size + 1, format, args);


  // if its first time adding to char, then you don't need to start with & so you don't allocate as much as you normally would
  if (body->body == NULL)
    {
    // + 2 for =, \0
    body->body = malloc(total_size + 2);
    snprintf(body->body, total_size + 2, "%s=%s", item, format_char);
  } else
  {
    size_t new_total_size = strlen(body->body) + total_size;
    // + 3 for &, =, and \0
    body->body = realloc(body->body, new_total_size + 3);
    char *body_copy = body->body;
    snprintf(body->body, new_total_size + 3, "%s&%s=%s", body_copy, item, format_char);
  }
  
  va_end(args);
  free(format_char);
}

void spotify_body_free(spotify_body *body)
{
    free(body->body);
    free(body);
}

spotify_header *spotify_new_header() {

  spotify_header *header = malloc(sizeof(*header));
  if (!header)
    return NULL;

  struct curl_slist *curl_header = NULL;
  header->curl_header = curl_header;
  return header;
}

// add two items at once
void spotify_header_add(spotify_header *header, char *item, char *format, ...) {

  va_list args;

  va_start(args, format);
  int args_size = vsnprintf(NULL, 0, format, args);
  va_end(args);

  size_t total_size = args_size + strlen(item);
  char *format_char = (char *)malloc(args_size + 1);

  va_start(args, format);
  vsnprintf(format_char, args_size + 1, format, args);

  // + 3 for :, space, \0
  char *content = (char *)malloc(total_size + 3);
  // + 3 for :, space, \0
  snprintf(content, total_size + 3, "%s: %s", item, format_char);
  free(format_char);
  header->curl_header = curl_slist_append(header->curl_header, content);

  free(content);
}

void spotify_header_free(spotify_header *header) {
  curl_slist_free_all(header->curl_header);
  free(header);
}


spotify_http spotify_send_http(char *url, char *request_type, spotify_body *body,
                            spotify_header *header) {
  CURLcode ret;
  CURL *hnd;
  char *response = NULL;

  hnd = curl_easy_init();

  curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(hnd, CURLOPT_URL, url);
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
  if (body != NULL)
  {
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, body->body);
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)strlen(body->body));
  } else 
  {
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, "");
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, 0L);
  }
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, header->curl_header);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/8.9.1");
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, request_type);
  curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, writefunc);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &response);

  ret = curl_easy_perform(hnd);
  curl_easy_cleanup(hnd);
  hnd = NULL;
  //not sure why its {0}, but it works rather just fine
  spotify_http res = {0};
  res.response = response;

  return res;
}

void spotify_http_free(spotify_http *result) { free(result->response); }

void spotify_encode(spotify_login *login, char *client_id, char *client_secret)
{
    size_t size = strlen(client_id) + strlen(client_secret);
    char *full_string = (char *)malloc(size + 2);
    // + 2 because of : and \0;
    snprintf(full_string, size + 2, "%s:%s", client_id, client_secret);
    char *encoded_id = base64_encode(full_string);
    login->encoded_id = encoded_id;
    free(full_string);
}