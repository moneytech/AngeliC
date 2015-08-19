#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "buffer.h"

angelic_buffer *
angelic_buffer_malloc(size_t len) {
    angelic_buffer* ret = malloc(sizeof(angelic_buffer));

    if(!ret){
        fprintf(stderr, "[AngeliC] Allocation failed.\n");
        abort();
    }

    ret->data = NULL;
    ret->size = ret->asize = 0;
    return ret;
}

angelic_buffer*
angelic_buffer_slice(angelic_buffer* ib, size_t offset, size_t len){
    angelic_buffer* ob = angelic_buffer_malloc(64);

    for(size_t i = 0; i < len; i++){
        angelic_buffer_putc(ob, ib->data[offset + i]);
    }

    return ob;
}

angelic_buffer*
angelic_buffer_wrap(const char* str){
    angelic_buffer* ret = angelic_buffer_malloc(64);
    angelic_buffer_puts(ret, str);
    return ret;
}

const char*
angelic_buffer_cstr(angelic_buffer* ib){
    if(ib->size < ib->asize && ib->data[ib->size] == 0){
        return (char*) ib->data;
    }

    angelic_buffer_grow(ib, ib->size + 1);
    ib->data[ib->size] = 0;
    return (const char *) ib->data;
}

uint8_t
angelic_buffer_getc(angelic_buffer* ib, size_t index){
    if(index > ib->size || index < 0){
        return 0;
    }

    return ib->data[index];
}

int
angelic_buffer_putf(angelic_buffer* ob, const FILE* file){
    while(!(feof((FILE *) file) || ferror((FILE *) file))){
        angelic_buffer_grow(ob, ob->size + 1024);
        ob->size += fread(ob->data + ob->size, 1, 1024, (FILE *) file);
    }

    return ferror((FILE *) file);
}

int
angelic_buffer_equals(angelic_buffer* b1, angelic_buffer* b2){
    if(b1->size != b2->size){
        return 0;
    }

    for(size_t i = 0; i < b1->size; i++){
        if(b1->data[i] != b2->data[i]){
            return 0;
        }
    }

    return 1;
}

int
angelic_buffer_starts_with(angelic_buffer* b1, angelic_buffer* b2){
    if(b1->size >= b2->size){
        for(size_t i = 0; i < b2->size; i++){
            if(b1->data[i] != b2->data[i]){
                fprintf(stdout, "%c != %c\n", b1->data[i], b2->data[i]);
                return 0;
            }
        }

        return 1;
    }

    return 0;
}

void
angelic_buffer_free(angelic_buffer* ib){
    if(!ib){
        return;
    }

    free(ib->data);
    free(ib);
}

void
angelic_buffer_grow(angelic_buffer* ib, size_t size){
    if(ib->asize >= size){
        return;
    }

    size_t nasize = ib->asize + 10;
    while(nasize < size){
        nasize += 10;
    }

    uint8_t* ret = realloc(ib->data, nasize);
    if(!ret){
        fprintf(stderr, "[AngeliC] Allocation failed.\n");
        abort();
    }
    ib->data = ret;
    ib->asize = nasize;
}

void
angelic_buffer_put(angelic_buffer* ob, const uint8_t* data, size_t size){
    if(ob->size + size > ob->asize){
        angelic_buffer_grow(ob, ob->size + size);
    }

    memcpy(ob->data + ob->size, data, size);
    ob->size += size;
}

void
angelic_buffer_putb(angelic_buffer* ob, angelic_buffer* ib){
    angelic_buffer_put(ob, ib->data, ib->size);
}

void
angelic_buffer_puts(angelic_buffer* ob, const char* str){
    angelic_buffer_put(ob, (const uint8_t*) str, strlen(str));
}

void
angelic_buffer_putc(angelic_buffer* ob, const uint8_t c){
    if(ob->size + 1 >= ob->asize){
        angelic_buffer_grow(ob, ob->size + 1);
    }

    ob->data[ob->size] = c;
    ob->size++;
}

void
angelic_buffer_putc_rep(angelic_buffer* ob, const uint8_t c, size_t times){
    for(size_t i = 0; i < times; i++){
        angelic_buffer_putc(ob, c);
    }
}

void
angelic_buffer_printf(angelic_buffer* ob, const char* fmt, ...){
    if(ob->size + 1 >= ob->asize){
        angelic_buffer_grow(ob, ob->size + 1);
    }

    va_list args_list;
    int args_len;

    va_start(args_list, fmt);
    args_len = vsnprintf((char*) ob->data + ob->size, ob->asize - ob->size, fmt, args_list);
    va_end(args_list);

    if(args_len < 0){
        return;
    }

    if(args_len >= ob->asize - ob->size){
        angelic_buffer_grow(ob, ob->size + args_len + 1);
        va_start(args_list, fmt);
        args_len = vsnprintf((char*) ob->data + ob->size, ob->asize - ob->size, fmt, args_list);
        va_end(args_list);
    }

    if(args_len < 0){
        return;
    }

    ob->size += args_len;
}

void
angelic_buffer_slurp(angelic_buffer* ob, size_t size){
    if(size >= ob->size){
        ob->size = 0;
        return;
    }

    ob->size -= size;
    memmove(ob->data, ob->data + size, ob->size);
}