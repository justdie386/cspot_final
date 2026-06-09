#ifndef TRACK_H
#define TRACK_H

typedef struct
{
    char *track_name;
    int track_id;
    char *album_name;
    int album_id;
    char *artist_name;
    int artist_id;
    int is_playing;
    // not sure if int is correct type for progress_ms, to look into
    int progress_ms;
} spotify_track;

spotify_track *spotify_new_track();

void spotify_track_free(spotify_track *track);

#endif // TRACK_H