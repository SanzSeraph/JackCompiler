#include <stdlib.h>
#include "dynamic_array.h"

struct DynamicArray* DynamicArray_new(size_t partitionSize)
{
    struct DynamicArray* self = (struct DynamicArray*)malloc(sizeof(struct DynamicArray));
    self->partitionSize = partitionSize;
    self->partitions = malloc(sizeof(self->partitions));
    self->partitionCount = 1;
    *self->partitions = calloc(self->partitionSize, sizeof(void*));
    self->currentEnd = 0;

    return self;
}

void DynamicArray_free(struct DynamicArray* self)
{
    for (size_t i = 0; i < self->partitionCount; i++) {
        free(self->partitions[i]);
    }

    free(self->partitions);
    free(self);
}

struct DynamicArrayResult DynamicArray_get(struct DynamicArray* self, size_t index)
{
    struct DynamicArrayResult result;

    if (index >= self->partitionSize * self->partitionCount) {
        result = (struct DynamicArrayResult){
            DYNAMIC_ARRAY_ERROR_OUT_OF_BOUNDS, NULL
        };
        goto ret;
    }

    size_t partitionIndex = index / self->partitionSize;
    size_t relativeIndex = index >= self->partitionSize ? index - self->partitionSize * (partitionIndex + 1) : index; //0123456789|0123456789|01234567890

    void** partition = self->partitions[partitionIndex];
    void* element = partition[relativeIndex];

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

    if (index >= self->partitionCount * self->partitionSize) {
        code = DYNAMIC_ARRAY_ERROR_OUT_OF_BOUNDS;
        goto ret;
    }

    size_t partitionIndex = index / self->partitionSize;
    size_t relativeIndex = index >= self->partitionSize ? index - self->partitionSize * (partitionIndex + 1) : index;

    void** partition = self->partitions[partitionIndex];
    partition[relativeIndex] = elem;

ret:
    return code;
}

enum DynamicArrayErrorCode DynamicArray_add(struct DynamicArray* self, void* elem)
{
    enum DynamicArrayErrorCode code = DYNAMIC_ARRAY_SUCCESS;

    if (self->currentEnd >= self->partitionCount * self->partitionSize) {
        self->partitionCount++;
        self->partitions = realloc(self->partitions, self->partitionCount);

        if (self->partitions == NULL) {
            code = DYNAMIC_ARRAY_ERROR_MEMORY_ALLOCATION;
            goto ret;
        }

        self->partitions[self->partitionCount - 1] = calloc(self->partitionSize, sizeof(void*));

        if (self->partitions[self->partitionCount - 1] == NULL) {
            code = DYNAMIC_ARRAY_ERROR_MEMORY_ALLOCATION;
            goto ret;
        }
    }

    DynamicArray_set(self, self->currentEnd, elem);
    self->currentEnd++;

ret:
    return code;
}

size_t DynamicArray_size(struct DynamicArray* self)
{
    return self->partitionCount * self->partitionSize;
}

size_t DynamicArray_count(struct DynamicArray* self)
{
    size_t count = 0;

    for (size_t i = 0; i < self->partitionCount; i++) {
        for (size_t j = 0; j < self->partitionSize; j++) {
            if (self->partitions[i][j] != NULL) {
                count++;
            }
        }
    }

    return count;
}