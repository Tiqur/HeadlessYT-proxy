#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "include/ws.h"

void onopen(ws_cli_conn_t *client)
{
  char *cli;
  cli = ws_getaddress(client);
  printf("Connection opened, addr: %s\n", cli);
  sleep(2);
  ws_sendframe_txt(client, "pause-play");
}

void onclose(ws_cli_conn_t *client)
{
    char *cli;
    cli = ws_getaddress(client);
    printf("Connection closed, addr: %s\n", cli);
}

void onmessage(ws_cli_conn_t *client,
    const unsigned char *msg, uint64_t size, int type)
{
    char *cli;
    cli = ws_getaddress(client);
    printf("I receive a message: %s (%zu), from: %s\n", msg,
        size, cli);
}
