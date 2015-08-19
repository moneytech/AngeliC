#include <stdlib.h>
#include "http.h"

angelic_http_header*
angelic_http_header_malloc(const char* key, const char* value){
    angelic_http_header* header = malloc(sizeof(angelic_http_header));
    header->key = angelic_buffer_wrap(key);
    header->value = angelic_buffer_wrap(value);
    return header;
}

void
angelic_http_header_free(angelic_http_header* header){
    if(header->key){
        angelic_buffer_free(header->key);
    }

    if(header->value){
        angelic_buffer_free(header->value);
    }

    free(header);
}

angelic_buffer*
angelic_http_header_cstr(angelic_http_header* header){
    angelic_buffer* buffer = angelic_buffer_malloc(64);
    angelic_buffer_printf(buffer, "%s: %s", header->key->data, header->value->data);
    return buffer;
}

angelic_http_response*
angelic_http_response_malloc(int status, const char* title){
    angelic_http_response* resp = malloc(sizeof(angelic_http_response));
    resp->content = angelic_buffer_malloc(1024);
    resp->title = angelic_buffer_wrap(title);
    resp->headers_len = 0;
    resp->status = status;
    return resp;
}

void
angelic_http_response_free(angelic_http_response* resp){
    for(size_t i = 0; i < resp->headers_len; i++) {
        if (resp->headers[i]) {
            angelic_http_header_free(resp->headers[i]);
        }
    }

    angelic_buffer_free(resp->content);
    free(resp);
}

void
angelic_http_response_add_header(angelic_http_response* resp, const char* key, const char* value){
    angelic_http_header* header = angelic_http_header_malloc(key, value);
    resp->headers[resp->headers_len] = header;
    resp->headers_len++;
}

angelic_buffer*
angelic_http_response_cstr(angelic_http_response* resp){
    angelic_buffer* buffer = angelic_buffer_malloc(1024);
    angelic_buffer_printf(buffer, "HTTP/1.1 %d %s\r\n", resp->status, angelic_buffer_cstr(resp->title));

    for(int i = 0; i < resp->headers_len; i++){
        angelic_buffer_puts(buffer, angelic_buffer_cstr(angelic_http_header_cstr(resp->headers[i])));
        angelic_buffer_puts(buffer, "\r\n");
    }

    angelic_buffer_puts(buffer, "\r\n");
    angelic_buffer_putb(buffer, resp->content);
    angelic_buffer_puts(buffer, "\r\n");
    return buffer;
}

angelic_http_request*
angelic_http_request_malloc(){
    angelic_http_request* request = malloc(sizeof(angelic_http_request));
    request->headers_len = 0;
    return request;
}

void
angelic_http_request_free(angelic_http_request* request){
    for(size_t i = 0; i < request->headers_len; i++){
        if(request->headers[i]){
            angelic_http_header_free(request->headers[i]);
        }
    }

    free(request);
}