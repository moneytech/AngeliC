#include "angelic.h"

angelic_router*
new_router(const char* name){
    angelic_buffer* name_buffer = angelic_buffer_wrap(name);
    angelic_router* router = angelic_router_malloc(name_buffer);
    angelic_buffer_free(name_buffer);
    return router;
}

angelic_route*
new_route(const char* name){
    angelic_buffer* name_buffer = angelic_buffer_wrap(name);
    angelic_route* route = angelic_route_malloc(name_buffer);
    angelic_buffer_free(name_buffer);
    return route;
}

angelic_http_response*
handle_home(angelic_http_request* req){
    angelic_http_response* resp = angelic_http_response_malloc(200, "OK");
    angelic_http_response_add_header(resp, "Content-Type", "text/html");
    angelic_http_response_add_header(resp, "Connection", "close");
    angelic_http_response_add_header(resp, "Server", "AngeliC");
    angelic_buffer_printf(resp->content, "<h1>Hello World</h1>");
    return resp;
}

int main(int argc, char** argv){
    angelic_route* route = new_route("");
    route->handle = &handle_home;

    angelic_router* router = new_router("/");
    angelic_router_route(router, route);

    angelic_http_server* server = angelic_http_server_malloc();
    angelic_http_server_use(server, router);
    angelic_http_server_start(server, "9944");
    angelic_http_server_listen(server);
    angelic_http_server_stop(server);
    angelic_http_server_free(server);
    return 0;
}