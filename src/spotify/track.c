#include <cjson/cJSON.h>
#include <spotify/track.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

spotify_track *spotify_new_track() {
  spotify_track *track = malloc(sizeof(*track));
  return track;
}

// Method to handle the json and put the data where data goes
// This shouldn't be something spotify.c takes care of in my opinion
void spotify_track_construct(spotify_track *track, char *response) {
  cJSON *root = cJSON_Parse(response);

  cJSON *is_playing = cJSON_GetObjectItem(root, "is_playing");
  track->is_playing = cJSON_IsTrue(is_playing);

  cJSON *item = cJSON_GetObjectItem(root, "item");
  cJSON *name = cJSON_GetObjectItem(item, "name");

  if (cJSON_IsString(name))
  {
    track->track_name = (char *)malloc(strlen(name->valuestring) + 1);
    strcpy(track->track_name, name->valuestring);
  } else 
  {
    track->track_name = NULL;
  }

  
  cJSON *artists = cJSON_GetObjectItem(item, "artists");
  cJSON *artist_object = cJSON_GetArrayItem(artists, 0);
  cJSON *artist_name = cJSON_GetObjectItem(artist_object, "name");


  if (cJSON_IsString(artist_name))
  {
    track->artist_name = (char *)malloc(strlen(artist_name->valuestring) + 1);
    strcpy(track->artist_name, artist_name->valuestring);
  }
  cJSON_Delete(root);
}

// Not really sure if i am freeing everything properly, should've just used C++, damn
void spotify_track_free(spotify_track *track) {
  free(track->track_name);
  free(track->artist_name);
}
