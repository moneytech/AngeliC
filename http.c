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
    request->method = angelic_buffer_malloc(10);
    request->route = angelic_buffer_malloc(100);
    request->body = angelic_buffer_malloc(100);
    return request;
}

void
angelic_http_request_add_header(angelic_http_request* request, angelic_http_header* header){
    request->headers[request->headers_len] = header;
    request->headers_len++;
}

void
angelic_http_request_free(angelic_http_request* request){
    for(size_t i = 0; i < request->headers_len; i++){
        if(request->headers[i]){
            angelic_http_header_free(request->headers[i]);
        }
    }

    angelic_buffer_free(request->route);
    angelic_buffer_free(request->body);
    angelic_buffer_free(request->method);
    free(request);
}

angelic_http_request_parser*
angelic_http_request_parser_malloc(angelic_buffer* ib){
    angelic_http_request_parser* parser = malloc(sizeof(angelic_http_request_parser));
    parser->content = ib;
    parser->pos = 0;
    return parser;
}

uint8_t
next_char(angelic_http_request_parser* parser){
    uint8_t c = parser->content->data[parser->pos];
    parser->pos++;
    return c;
}

angelic_buffer*
next_line(angelic_http_request_parser* parser){
    angelic_buffer* buffer = angelic_buffer_malloc(100);

    uint8_t c;
    while((c = next_char(parser)) != '\n'){
        angelic_buffer_putc(buffer, c);
    }

    return buffer;
}

angelic_buffer*
first_token(angelic_buffer* buffer){
    angelic_buffer* tmp = angelic_buffer_malloc(buffer->size);
    uint8_t c;
    size_t pos = 0;
    while((c = buffer->data[pos++]) != ' '){
        angelic_buffer_putc(tmp, c);
    }
    return tmp;
}

angelic_http_request*
angelic_http_request_parser_parse(angelic_http_request_parser* parser){
    angelic_http_request* request = angelic_http_request_malloc();
    angelic_buffer* line = next_line(parser);

    if(angelic_buffer_starts_with_str(line, "GET")){
        request->method = angelic_buffer_wrap("GET");
    } else if(angelic_buffer_starts_with_str(line, "POST")){
        request->method = angelic_buffer_wrap("POST");
    } else{
        fprintf(stderr, "Unknown request method: %s\n", angelic_buffer_cstr(line));
        abort();
    }

    parser->pos += line->size;

    angelic_buffer* tmp = angelic_buffer_slice(line, request->method->size + 1, line->size - 1 - request->method->size);
    angelic_buffer* route = first_token(tmp);
    angelic_buffer_putb(request->route, route);
    angelic_buffer_free(route);
    angelic_buffer_free(line);
    angelic_buffer_free(tmp);

    uint8_t c;
    while((c = next_char(parser)) != '\n'){
        angelic_buffer* key = angelic_buffer_malloc(100);
        angelic_buffer* value = angelic_buffer_malloc(100);

        angelic_buffer_putc(key, c);

        while((c = next_char(parser)) != ':'){
            angelic_buffer_putc(key, c);
        }

        parser->pos++;

        while((c = next_char(parser)) != '\n'){
            angelic_buffer_putc(value, c);
        }

        angelic_http_header* header = angelic_http_header_malloc(angelic_buffer_cstr(key), angelic_buffer_cstr(value));
        angelic_http_request_add_header(request, header);
    }

    while((c = next_char(parser)) != -1){
        angelic_buffer_putc(request->body, c);
    }

    return request;
}

void
angelic_http_request_parser_free(angelic_http_request_parser* parser){
    angelic_buffer_free(parser->content);
    free(parser);
}