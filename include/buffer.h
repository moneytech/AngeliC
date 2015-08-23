#ifndef ANGELIC_BUFFER_H
#define ANGELIC_BUFFER_H

#include <stdint-gcc.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

#define __attribute__(x)

struct angelic_buffer{
    uint8_t* data;

    size_t size;
    size_t asize;
};

typedef struct angelic_buffer angelic_buffer;

angelic_buffer* angelic_buffer_malloc(size_t size) __attribute__((malloc));
angelic_buffer* angelic_buffer_slice(angelic_buffer* ib, size_t offset, size_t len);
angelic_buffer* angelic_buffer_wrap(const char* str);
const char* angelic_buffer_cstr(angelic_buffer* ib);
uint8_t angelic_buffer_getc(angelic_buffer* ib, size_t index);
int angelic_buffer_putf(angelic_buffer* ob, const FILE* file);
int angelic_buffer_starts_with(angelic_buffer* b1, angelic_buffer* b2);
int angelic_buffer_starts_with_str(angelic_buffer* b1, const char* str);
int angelic_buffer_equals(angelic_buffer* b1, angelic_buffer* b2);
void angelic_buffer_free(angelic_buffer* ib);
void angelic_buffer_grow(angelic_buffer* ib, size_t size);
void angelic_buffer_put(angelic_buffer* ob, const uint8_t* data, size_t size);
void angelic_buffer_putb(angelic_buffer* ob, angelic_buffer* ib);
void angelic_buffer_puts(angelic_buffer* ob, const char* str);
void angelic_buffer_putc(angelic_buffer* ob, const uint8_t c);
void angelic_buffer_putc_rep(angelic_buffer* ob, const uint8_t c, size_t times);
void angelic_buffer_printf(angelic_buffer* ob, const char* fmt, ...) __attribute__((format));
void angelic_buffer_slurp(angelic_buffer* ob, size_t size);

#ifdef __cplusplus
};
#endif

#endif