#include "spotify/track.h"
#include <spotify/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <spotify/spotify.h>

void spotify_login_free(spotify_login *login)
{
    if (login->access_token != NULL)
        free(login->access_token);
    if (login->authorization_token != NULL)
        free(login->authorization_token);
    if (login->encoded_id != NULL)
        free(login->encoded_id);
    if (login->refresh_token != NULL)
        free(login->refresh_token);
}

spotify_track *spotify_get_current_track(char *authorization_code)
{
    spotify_track *track = spotify_new_track();

    spotify_header *header = spotify_new_header();
    spotify_header_add(header, "Authorization", "Bearer %s", authorization_code);

    spotify_http response = spotify_send_http("https://api.spotify.com/v1/me/player/currently-playing", "GET", NULL, header);

    spotify_track_construct(track, response.response);

    spotify_header_free(header);
    spotify_http_free(&response);
    return track;
}