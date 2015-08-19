#include <stdlib.h>
#include <string.h>
#include "router.h"

angelic_route*
angelic_route_malloc(angelic_buffer* name){
    angelic_route* ret = malloc(sizeof(angelic_route));

    if(!ret){
        fprintf(stderr, "[AngeliC] Allocation Failed.\n");
        abort();
    }

    ret->name = angelic_buffer_malloc(64);
    angelic_buffer_putb(ret->name, name);
    return ret;
}

void
angelic_route_free(angelic_route* route){
    angelic_buffer_free(route->name);
    free(route);
}

angelic_router*
angelic_router_malloc(angelic_buffer* name){
    angelic_router* ret = malloc(sizeof(angelic_router));

    if(!ret){
        fprintf(stderr, "[AngeliC] Allocation Failed.\n");
        abort();
    }

    ret->sub_routes_size = 0;
    ret->name = angelic_buffer_malloc(64);
    angelic_buffer_putb(ret->name, name);
    return ret;
}

void
angelic_router_free(angelic_router* router){
    angelic_buffer_free(router->name);
    for(size_t i = 0; i < 100; i++){
        if(router->sub_routes[i]){
            angelic_route_free(router->sub_routes[i]);
        }
    }
    free(router);
}

void
angelic_router_route(angelic_router* router, angelic_route* route){
    router->sub_routes[router->sub_routes_size] = route;
    router->sub_routes_size++;
}

angelic_route*
angelic_router_find(angelic_router* router, angelic_buffer* route){
    if(angelic_buffer_starts_with(route, router->name)){
        angelic_buffer* new_route = angelic_buffer_slice(route, router->name->size, route->size - router->name->size);

        for(size_t i = 0; i < 100; i++){
            if(angelic_buffer_equals(new_route, router->sub_routes[i]->name)){
                angelic_buffer_free(new_route);
                return router->sub_routes[i];
            }
        }

        angelic_buffer_free(new_route);
        return NULL;
    } else{
        for(size_t i = 0; i < 100; i++){
            if(angelic_buffer_equals(route, router->sub_routes[i]->name)){
                return router->sub_routes[i];
            }
        }

        return NULL;
    }
}