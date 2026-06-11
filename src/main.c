#include <stdio.h>
#include <stdlib.h>
#include <spotify/spotify.h>
#include <spotify/utils.h>
#include <spotify/auth.h>
#include <spotify/config.h>
#include <time.h>

#define CLIENT_ID "1d76c1f4d5e04cb689e6bc7d8427fe97"
#define CLIENT_SECRET "cf29197bd5114488ab24c4c3500d71bd"

int main()
{
    spotify_login login;

    char *folder_path = spotify_new_config_path("%s%s", getenv("HOME"), "/.config/cspotify");
    char *file_path = spotify_new_config_path("%s%s", folder_path, "/config.json");
    if (spotify_config_exist(folder_path) == 0)
    {
        printf("[+] Config file found!\n");
        spotify_config_open(&login, file_path);
        spotify_refresh_access_token(&login, login.encoded_id, login.refresh_token);
    } 
    else 
    {
        printf("[+] Encoding spotify ID\n");
        spotify_encode(&login, CLIENT_ID, CLIENT_SECRET);
        printf("[+] Encoded id: %s\n", login.encoded_id);
        printf("[+] Generating authorization url callback\n");

        // the spotify web api requires us to have a random number between 100000 and 999999 at the end of the url we are making the user open
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

        spotify_config_create(folder_path, file_path, login.refresh_token, login.encoded_id);
    }

    spotify_track *current_song = spotify_get_current_track(login.access_token);

    if (current_song->is_playing == 1)
    {
        printf("[+] Current song playing: %s\n", current_song->track_name);
     } else 
     {
         printf("[+] No song currently playing/issue talking with spotify?\n");
     }

    free(folder_path);
    free(file_path);
    spotify_login_free(&login);
}