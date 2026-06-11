#include "spotify/spotify.h"
#include <spotify/auth.h>
#include <spotify/utils.h>
#include <time.h>
#include <cjson/cJSON.h>
#include <mongoose.c>

char *auth_code;
// tells the server when to stop running
int running = 0;

void ev_handler(struct mg_connection *c, int ev, void *ev_data)
{
  if (ev == MG_EV_HTTP_MSG)
    {
      struct mg_http_message *hm = (struct mg_http_message *) ev_data;
      struct mg_str val = mg_http_var(hm->query, mg_str("code"));
      if (val.len != 0)
	{
          auth_code = (char *)malloc(val.len + 1);
          mg_http_get_var(&hm->query, "code", auth_code, val.len + 1);
          mg_http_reply(c, 200, "Hello", "Auth code received\n");
          running = 1;
	}
      //not really sure what to do when this occurs to be honest
    }
}

// I took this method from an old project of mine, not sure how good or safe it is, but it works
void spotify_fetch_authorization_token(spotify_login *login)
{
      struct mg_mgr mgr;
  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, "http://127.0.0.1:8888/callback", ev_handler, NULL);
  while (running == 0)
    {
      mg_mgr_poll(&mgr, 1000);
    }
  login->authorization_token = (char *)malloc(strlen(auth_code) + 1);
  strcpy(login->authorization_token, auth_code);
  mg_mgr_free(&mgr);
  free(auth_code);
}


void spotify_fetch_access_token_and_refresh_token(spotify_login *login, char *encoded_id, char *authorization_token)
{
    spotify_header *header = spotify_new_header();
    spotify_header_add(header, "Authorization", "Basic %s", encoded_id);
    spotify_header_add(header, "Content-Type", "application/x-www-form-urlencoded");

    // I made spotify_*body/header*_add this way as a sort of helper. I don't trust myself manually allocating size for each of these strings
    // each and every single i need to combine two strings together
    // this makes cleaning up everything a whole lot easier, and a whole lot more readable, at least to me
    spotify_body *body = spotify_new_body();
    spotify_body_add(body, "grant_type", "%s", "authorization_code");
    spotify_body_add(body, "code", "%s", authorization_token);
    spotify_body_add(body, "redirect_uri", "%s", "http://127.0.0.1:8888/callback");

    spotify_http http = spotify_send_http("https://accounts.spotify.com/api/token", "POST", body, header);

    cJSON *root = cJSON_Parse(http.response);

    cJSON *access_token = cJSON_GetObjectItem(root, "access_token");
    cJSON *refresh_token = cJSON_GetObjectItem(root, "refresh_token");

    login->access_token = (char *)malloc(strlen(access_token->valuestring) + 1);
    strcpy(login->access_token, access_token->valuestring);

    login->refresh_token = (char *)malloc(strlen(refresh_token->valuestring) + 1);
    strcpy(login->refresh_token, refresh_token->valuestring);

    cJSON_free(root);
    spotify_body_free(body);
    spotify_header_free(header);
    spotify_http_free(&http);
}

// method used not to GET the refresh token, but to REFRESH the access token, using the refresh token
void spotify_refresh_access_token(spotify_login *login, char *encoded_id, char *refresh_token)
{
  spotify_header *header = spotify_new_header();
  spotify_header_add(header, "Content-Type", "%s", "application/x-www-form-urlencoded");
  spotify_header_add(header, "Authorization", "Basic %s", encoded_id);

  spotify_body *body = spotify_new_body();
  spotify_body_add(body, "grant_type", "%s", "refresh_token");
  spotify_body_add(body, "refresh_token", "%s", refresh_token);

  spotify_http http = spotify_send_http("https://accounts.spotify.com/api/token", "POST", body, header);

  cJSON *root = cJSON_Parse(http.response);
  cJSON *access_token = cJSON_GetObjectItem(root, "access_token");

  login->access_token = (char *)malloc(strlen(access_token->valuestring) +1);
  strcpy(login->access_token, access_token->valuestring);
  spotify_header_free(header);
  spotify_body_free(body);
  spotify_http_free(&http);
}