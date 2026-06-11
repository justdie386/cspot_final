#ifndef CONFIG_H
#define CONFIG_H

#include <spotify/spotify.h>

// i dont want to have to add config.json to the config path using malloc
// so i just made two #define, if there's a better solution, let me know
#ifdef _WIN32
#define CSPOTIFY_CONFIG_PATH "something something appdata"
#define CSPOTIFY_CONFIG_FILE "Something something appdata"
#define mkdir(dir, mode), _mkdir(dir)
#else
#define CSPOTIFY_CONFIG_PATH getenv("HOME") + ".config/cspotify"
#define CSPOTIFY_CONFIG_FILE "~/.config/cspotify/config.json"
#endif

int spotify_config_exist(const char *filename);

void spotify_config_create(const char *folder, const char *filename, const char *refresh_token, const char *encoded_id);

void spotify_config_open(spotify_login *login, const char *filename);

char *spotify_new_config_path(char *format, ...);

#endif // CONFIG_H