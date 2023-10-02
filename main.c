#include <stdio.h>
#include <pthread.h>
#include "include/ws_server_events.h"
#include "include/mongoose.h"

char* message;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;

    cond = 
    pthread_cond_signal(&cond);

    if (mg_http_match_uri(hm, "/api/hello")) {              // On /api/hello requests,
      mg_http_reply(c, 200, "", "{%m:%d}\n",
                    MG_ESC("status"), 1);                   // Send dynamic JSON response
    } else {                                                // For all other URIs,
      struct mg_http_serve_opts opts = {.root_dir = "."};   // Serve files
      mg_http_serve_dir(c, hm, &opts);                      // From root_dir
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
  pthread_create(&thread_id, NULL, start_http_server, message);

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
