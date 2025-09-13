#include <stdlib.h>
#include <string.h>
#include "dynamic_array.h"

struct DynamicArray* DynamicArray_new(size_t initialSize)
{
    struct DynamicArray* self = (struct DynamicArray*)malloc(sizeof(struct DynamicArray));
    self->capacity = initialSize;
	self->array = calloc(initialSize, sizeof(void*));
    self->currentEnd = 0;

    return self;
}

void DynamicArray_free(struct DynamicArray* self)
{
    free(self->array);
    free(self);
}

struct DynamicArrayResult DynamicArray_get(struct DynamicArray* self, size_t index)
{
    struct DynamicArrayResult result;

    if (index >= self->capacity) {
        result = (struct DynamicArrayResult){
            DYNAMIC_ARRAY_ERROR_OUT_OF_BOUNDS, 
            NULL
        };
        goto ret;
    }
    
    void* element = self->array[index];
    
    result = (struct DynamicArrayResult){
        DYNAMIC_ARRAY_SUCCESS,
        element
    };

    ret:
    return result;
}

enum DynamicArrayErrorCode DynamicArray_set(struct DynamicArray* self, size_t index, void* elem)
{
    enum DynamicArrayErrorCode code = DYNAMIC_ARRAY_SUCCESS;

    if (index >= self->capacity) {
        code = DYNAMIC_ARRAY_ERROR_OUT_OF_BOUNDS;
        goto ret;
    }

    self->array[index] = elem;

    if (index > self->currentEnd) {
        self->currentEnd = index + 1;
	}

    ret:
    return code;
}

enum DynamicArrayErrorCode DynamicArray_add(struct DynamicArray* self, void* elem)
{
    enum DynamicArrayErrorCode code = DYNAMIC_ARRAY_SUCCESS;

    if (self->currentEnd >= self->capacity) {
        void **newArray = realloc(self->array, self->capacity * 2 * sizeof(void*));

        if (newArray == NULL) {
            code = DYNAMIC_ARRAY_ERROR_MEMORY_ALLOCATION;

            goto ret;
		}

		self->capacity *= 2;
        self->array = newArray;        
    }

    DynamicArray_set(self, self->currentEnd, elem);
    self->currentEnd++;

    ret:
    return code;
}