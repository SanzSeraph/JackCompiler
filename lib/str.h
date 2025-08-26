#ifndef STR_H
#define STR_H
#include <stddef.h>
#include <stdbool.h>

struct String;
typedef struct String* String_t;

String_t str_new(char* str, size_t len, bool copy);
void str_free(String_t s);

#endif // STR_H