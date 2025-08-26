#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
#include <stddef.h>

struct DynamicArray {
    size_t partitionSize;
    void ***partitions;
    size_t partitionCount;
    size_t currentEnd;
};

enum DynamicArrayErrorCode {
    DYNAMIC_ARRAY_SUCCESS = 0,
    DYNAMIC_ARRAY_ERROR_OUT_OF_BOUNDS = 1,
    DYNAMIC_ARRAY_ERROR_MEMORY_ALLOCATION = 2
};

struct DynamicArrayResult {
    enum DynamicArrayErrorCode code;
    void *data;
};

struct DynamicArray *DynamicArray_new(size_t partitionSize);
void DynamicArray_free(struct DynamicArray *self);
struct DynamicArrayResult DynamicArray_get(struct DynamicArray *self, size_t index);
enum DynamicArrayErrorCode DynamicArray_set(struct DynamicArray *self, size_t index, void *elem);
enum DynamicArrayErrorCode DynamicArray_add(struct DynamicArray *self, void *elem);
size_t DynamicArray_size(struct DynamicArray *self);

#endif // DYNAMIC_ARRAY_H