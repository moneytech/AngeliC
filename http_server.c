#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include "http_server.h"

angelic_http_server*
angelic_http_server_malloc(){
    angelic_http_server* ret = malloc(sizeof(angelic_http_server));

    if(!ret){
        fprintf(stderr, "[AngeliC] Allocation Failed.\n");
        abort();
    }

    ret->sock = -1;
    ret->routers_size = 0;
    for(size_t i = 0; i < HTTP_MAX_CONNECTIONS; i++){
        ret->clients[i] = -1;
    }
    return ret;
}

void
angelic_http_server_use(angelic_http_server* server, angelic_router* router){
    server->routers[server->routers_size] = router;
    server->routers_size++;
}

void
angelic_http_server_free(angelic_http_server* server){
    for(size_t i = 0; i < ROUTER_MAX; i++){
        if(server->routers[i]){
            angelic_router_free(server->routers[i]);
        }
    }

    free(server);
}

void
angelic_http_server_start(angelic_http_server* server, const char* port){
    struct sockaddr_in serv_addr;
    server->sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server->sock < 0){
        fprintf(stderr, "[AngeliC] can't open server socket\n");
        abort();
    }
    setsockopt(server->sock, SOL_SOCKET, SO_REUSEADDR, (const void *) 1, sizeof(int));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons((uint16_t) atoi(port));
    if(bind(server->sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1){
        close(server->sock);
        fprintf(stderr, "Can't bind socket\n");
        abort();
    }
}

static void
angelic_write_response(int client, int status, const char* title){
    angelic_buffer* resp_buffer = angelic_buffer_malloc(64);
    angelic_buffer_printf(resp_buffer, "HTTP/1.1 %d %s\r\n", status, title);
    angelic_buffer_puts(resp_buffer, "Server: AngeliC\r\n");
    angelic_buffer_puts(resp_buffer, "Connection: close\r\n\r\n");
    angelic_buffer_puts(resp_buffer, "<h1>Hello World</h1>\r\n");
    write(client, resp_buffer->data, resp_buffer->size);
}

static void
process(int client, angelic_http_server* server){
    angelic_buffer* request_route = angelic_buffer_wrap("/");
    for(size_t i = 0; i < server->routers_size; i++){
        angelic_route* route = angelic_router_find(server->routers[i], request_route);
        if(route){
            angelic_http_request* req = angelic_http_request_malloc();
            angelic_http_response* resp = route->handle(req);
            angelic_http_request_free(req);
            angelic_buffer* resp_buffer = angelic_http_response_cstr(resp);
            fprintf(stdout, angelic_buffer_cstr(resp_buffer));
            write(client, resp_buffer->data, resp_buffer->size);
            angelic_http_response_free(resp);
        }
    }
    angelic_buffer_free(request_route);
}

void
angelic_http_server_listen(angelic_http_server* server){
    while(1){
        if(listen(server->sock, 10) < 0){
            fprintf(stderr, "[AngeliC] listen()\n");
            abort();
        }

        int client = accept(server->sock, NULL, NULL);
        if(client < 0){
            fprintf(stderr, "[AngeliC] accept() error\n");
            continue;
        }
        process(client, server);
        close(client);
    }
}

void
angelic_http_server_stop(angelic_http_server* server){
    close(server->sock);
}