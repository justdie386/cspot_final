#include <stdio.h>
#include <stdlib.h>
#include <spotify/spotify.h>
#include <spotify/utils.h>
#include <spotify/auth.h>
#include <time.h>
#define CLIENT_ID "5e788971862242c2ac6105e2782ce87c"
#define CLIENT_SECRET "12465d86180048f3b03af72dc1ce5883"


int main()
{
    spotify_login login;

    printf("[+] Creating header\n");
    spotify_header *header = spotify_new_header();

    printf("[+] Alive check\n");

    printf("[+] Encoding spotify ID\n");
    spotify_encode(&login, CLIENT_ID, CLIENT_SECRET);
    printf("[+] Encoded id: %s\n", login.encoded_id);

    printf("[+] Alive check\n");

    printf("[+] Generating authorization url callback\n");

    srand(time(NULL));
    int code = 100000 + rand() % 900000;
    
    printf("[+] Fetching authorization token\n");
    printf("[+] Please open the following url: https://accounts.spotify.com/authorize?client_id=%s&response_type=code&scope=user-read-currently-playing&redirect_uri=http://127.0.0.1:8888/callback&state=%d\n",  CLIENT_ID, code);
    spotify_fetch_authorization_token(&login);

    printf("[+] Printing auth code\n%s\n", login.authorization_token);

    printf("[+] Fetching access token\n");
    spotify_fetch_access_token_and_refresh_token(&login, login.encoded_id, login.authorization_token);

    printf("[+] Access token: %s\n", login.access_token);
    printf("[+] Refresh token: %s\n", login.refresh_token);

    spotify_track *current_song = spotify_get_current_track(login.access_token);

    if (current_song->is_playing == 1)
    {
        printf("[+] Current song playing: %s\n", current_song->track_name);
    } else 
    {
        printf("[+] No song currently playing/issue talking with spotify?\n");
    }

    spotify_login_free(&login);
}