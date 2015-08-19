#ifndef ANGELIC_ROUTER_H
#define ANGELIC_ROUTER_H

#include "buffer.h"
#include "http.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SUBROUTE_MAX
#define SUBROUTE_MAX 100
#endif

struct angelic_route {
    angelic_buffer* name;
    angelic_http_response* (*handle)(angelic_http_request* request);
};

typedef struct angelic_route angelic_route;

angelic_route* angelic_route_malloc(angelic_buffer* name) __attribute__((malloc));
void angelic_route_free(angelic_route* route);

struct angelic_router{
    angelic_buffer* name;
    angelic_route* sub_routes[SUBROUTE_MAX];
    size_t sub_routes_size;
};

typedef struct angelic_router angelic_router;

angelic_router* angelic_router_malloc(angelic_buffer* name) __attribute__((malloc));
void angelic_router_free(angelic_router* router);
void angelic_router_route(angelic_router* router, angelic_route* route);
angelic_route* angelic_router_find(angelic_router* router, angelic_buffer* route);

#ifdef __cplusplus
};
#endif

#endif