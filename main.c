#include <stdio.h>
#include <pthread.h>
#include "include/ws.h"
#include "include/ws_server_events.h"
#include "include/mongoose.h"

//Command message;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
ws_cli_conn_t *global_client;

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;

    if (mg_http_match_uri(hm, "/pause-play")) {
      ws_sendframe_txt(global_client, "pause-play");
      mg_http_reply(c, 200, "", "", MG_ESC("status"), 1);
    } else 
    if (mg_http_match_uri(hm, "/mute")) {
      ws_sendframe_txt(global_client, "mute");
      mg_http_reply(c, 200, "", "", MG_ESC("status"), 1);
    } else 
    if (mg_http_match_uri(hm, "/next_video")) {
      ws_sendframe_txt(global_client, "next_video");
      mg_http_reply(c, 200, "", "", MG_ESC("status"), 1);
    }
    else
    if (mg_http_match_uri(hm, "/prev_video")) {
      ws_sendframe_txt(global_client, "prev_video");
      mg_http_reply(c, 200, "", "", MG_ESC("status"), 1);
    } else {
      mg_http_reply(c, 404, "", "", MG_ESC("status"), 1);
    }
  }
}

void *start_http_server()
{
  struct mg_mgr mgr;                                
  mg_mgr_init(&mgr);
  mg_http_listen(&mgr, "http://0.0.0.0:8000", fn, NULL);
  for (;;) mg_mgr_poll(&mgr, 1000);
}

int main(void)
{
  // Start HTTP server on seperate thread
  printf("Starting HTTP server...\n");
  pthread_t thread_id;
  pthread_create(&thread_id, NULL, start_http_server, NULL);

  // Init websocket 
  struct ws_events evs;
  evs.onopen = &onopen;
  evs.onclose = &onclose;
  evs.onmessage = &onmessage;

  // Start websocket server
  printf("Starting websocket server...\n");
  ws_socket(&evs, 3000, 0, 1000);
  return 0;
}
