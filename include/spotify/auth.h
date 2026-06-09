#ifndef SPOTIFY_AUTH_H
#define SPOTIFY_AUTH_H

#include <spotify/spotify.h>

void spotify_fetch_authorization_token(spotify_login *login);

void spotify_fetch_access_token_and_refresh_token(spotify_login *login, char *encoded_id, char *authorization_token);

#endif // SPOTIFY_AUTH_H