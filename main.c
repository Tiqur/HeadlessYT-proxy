#include <stdio.h>
#include "include/ws_server_events.h"


int main(void)
{
  struct ws_events evs;
  evs.onopen = &onopen;
  evs.onclose = &onclose;
  evs.onmessage = &onmessage;

  ws_socket(&evs, 3000, 0, 1000);
  return 0;
}
