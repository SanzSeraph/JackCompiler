#include "str.h"
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

struct String {
    char *data;
    size_t length;
};

String_t str_new(char* str, size_t len, bool copy) {
    String_t new_str = (String_t)malloc(sizeof(struct String));

    if (new_str == NULL) {
        goto ret;
    }

    if (copy) {
        new_str->data = (char*)malloc(len + 1);
        strncpy(new_str->data, str, len);
    } else {
        new_str->data = str;
    }

    new_str->length = len;

    ret:
    return new_str;
}