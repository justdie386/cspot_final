#ifndef SPOTIFY_UTILS_H
#define SPOTIFY_UTILS_H

#include <spotify/spotify.h>

typedef struct
{
    char *response;
} spotify_http;

typedef struct
{
  struct curl_slist* curl_header;
} spotify_header;

typedef struct
{
    char *body;
} spotify_body;

spotify_body *spotify_new_body();

void spotify_body_add(spotify_body *body, char *item, char *format, ...);

void spotify_body_free(spotify_body *body);

spotify_header *spotify_new_header();

void spotify_header_add(spotify_header *header, char *item, char *format, ...);

void spotify_header_free(spotify_header *heady);

spotify_http spotify_send_http(char *url, char *request_type, spotify_body *body,
                            spotify_header *header);

void spotify_http_free(spotify_http *http);

void spotify_encode(spotify_login *login, char *client_id, char *client_secret);

#endif // SPOTIFY_UTILS_H