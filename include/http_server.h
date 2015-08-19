#ifndef ANGELIC_HTTP_SERVER_H
#define ANGELIC_HTTP_SERVER_H

#include "router.h"

#ifndef HTTP_MAX_CONNECTIONS
#define HTTP_MAX_CONNECTIONS 1000
#endif

#define ROUTER_MAX 100

#ifndef __attribute__
#define __attribute__(x)
#endif

#define HTTP_SOK 200
#define HTTP_SBR 400
#define HTTP_SNF 404

struct angelic_http_server{
    size_t routers_size;
    int sock;
    int clients[HTTP_MAX_CONNECTIONS];
    angelic_router* routers[ROUTER_MAX];
};

typedef struct angelic_http_server angelic_http_server;

angelic_http_server* angelic_http_server_malloc() __attribute__((malloc));
void angelic_http_server_use(angelic_http_server* server, angelic_router* router);
void angelic_http_server_free(angelic_http_server* server);
void angelic_http_server_start(angelic_http_server* server, const char* port);
void angelic_http_server_listen(angelic_http_server* server);
void angelic_http_server_stop(angelic_http_server* server);

#endif