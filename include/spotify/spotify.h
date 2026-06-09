#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <spotify/track.h>

// TODO make sure all them are freed properly and when needed
typedef struct
{
    char *client_id;
    char *client_secret;
    char *access_token;
    char *refresh_token;
    char *authorization_token;
    char *encoded_id;

} spotify_login;

spotify_track *spotify_get_current_track(char *authorization_code);

void spotify_login_free(spotify_login *login);

void spotify_track_construct(spotify_track *track, char *response);
#endif // SPOTIFY_H