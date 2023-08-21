#include "mongoose.h"

struct mg_mgr mgr;


static void cb(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
  switch (ev)
  {
  case MG_EV_OPEN:
    break;
  case MG_EV_ACCEPT:
  {
    MG_INFO(("SERVER accepted a connection"));
    printf("for conns item!\n");
    for (struct mg_connection *item = mgr.conns->next; item != NULL; item = item->next)
    {
      printf("mg_connection item:%p is_listening:%d is_client:%d   ", item, item->is_listening, item->is_client);
    }
    printf("\n");
  }
  break;
  case MG_EV_READ:
  {
    mg_send(c, c->recv.buf, c->recv.len);   // Echo received data back
    mg_iobuf_del(&c->recv, 0, c->recv.len); // And discard it
  }
  break;
  case MG_EV_CLOSE:
    break;
  case MG_EV_ERROR:
    break;
  default:
    break;
  }
}

int main(int argc, char *argv[])
{
  
  mg_mgr_init(&mgr);                               // Init manager
  mg_listen(&mgr, "tcp://0.0.0.0:1234", cb, &mgr); // Setup listener

  for (;;)
  {
    mg_mgr_poll(&mgr, 1000); // Event loop
  }

  mg_mgr_free(&mgr); // Cleanup
  return 0;
}
