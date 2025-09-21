#include "dynamic_string.h"
#include <stdlib.h>

struct DynamicString* DynamicString_new(size_t initialSize)
{
    struct DynamicString* self = (struct DynamicString*)malloc(sizeof(struct DynamicString));

    if (self == NULL) {
		return NULL;
    }

    self->capacity = initialSize;
    self->array = calloc(initialSize, sizeof(char));
    self->currentEnd = 0;

    return self;
}

void DynamicString_free(struct DynamicString* self)
{
    free(self->array);
    free(self);
}

struct DynamicStringResult DynamicString_get(struct DynamicString* self, size_t index)
{
    struct DynamicStringResult result;

    if (index >= self->capacity) {
        result = (struct DynamicStringResult){
            DYNAMIC_STRING_ERROR_OUT_OF_BOUNDS,
            NULL
        };
        goto ret;
    }

    void* element = self->array[index];

    result = (struct DynamicStringResult){
        DYNAMIC_STRING_SUCCESS,
        element
    };

ret:
    return result;
}

enum DynamicStringErrorCode DynamicString_set(struct DynamicString* self, size_t index, char c)
{
    enum DynamicStringErrorCode code = DYNAMIC_STRING_SUCCESS;

    if (index >= self->capacity) {
        code = DYNAMIC_STRING_ERROR_OUT_OF_BOUNDS;
        goto ret;
    }

    self->array[index] = c;

    if (index > self->currentEnd) {
        self->currentEnd = index + 1;
    }

ret:
    return code;
}

enum DynamicStringErrorCode DynamicString_add(struct DynamicString* self, char c)
{
    enum DynamicStringErrorCode code = DYNAMIC_STRING_SUCCESS;

    if (self->currentEnd >= self->capacity) {
        void** newArray = realloc(self->array, self->capacity * 2 * sizeof(void*));

        if (newArray == NULL) {
            code = DYNAMIC_STRING_ERROR_MEMORY_ALLOCATION;

            goto ret;
        }

        self->capacity *= 2;
        self->array = newArray;
    }

    DynamicArray_set(self, self->currentEnd, c);
    self->currentEnd++;

ret:
    return code;
}