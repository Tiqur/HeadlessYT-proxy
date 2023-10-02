#include <signal.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "include/ws.h"
#include "include/ws_server_events.h"
#include "include/mongoose.h"

//Command message;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
ws_cli_conn_t *global_client;
int stop_flag = false;

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
    } else
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
  mg_http_listen(&mgr, "http://0.0.0.0:8111", fn, NULL);
  while (!stop_flag) {
    mg_mgr_poll(&mgr, 1000);
  }
  pthread_exit(NULL);
}

void *start_ws_server() {
  // Init websocket 
  struct ws_events evs;
  evs.onopen = &onopen;
  evs.onclose = &onclose;
  evs.onmessage = &onmessage;

  // Start websocket server
  ws_socket(&evs, 3000, 0, 1000);
}

void* start_heartbeat_provider() {
  // Ping client every x seconds to keep alive
  while (!stop_flag) {
    sleep(5);
    ws_sendframe_txt(global_client, "ping");
  }
  pthread_exit(NULL);
}

void signal_handler() {
  stop_flag = true;
}

int main(void)
{
  // Start HTTP server on seperate thread
  printf("Starting HTTP server...\n");
  pthread_t http_thread_id;
  pthread_create(&http_thread_id, NULL, start_http_server, NULL);

  // Start WebSocket server on seperate thread
  printf("Starting WebSocket server...\n");
  pthread_t ws_thread_id;
  pthread_create(&ws_thread_id, NULL, start_ws_server, NULL);

  // Start heartbeat provider
  printf("Starting HeartBeat provider...\n");
  pthread_t hbp_thread_id;
  pthread_create(&hbp_thread_id, NULL, start_heartbeat_provider, NULL);

  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  while(!stop_flag);

  return 0;
}
