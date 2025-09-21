#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

struct DynamicString {
    size_t capacity;
    char* array;
    size_t currentEnd;
};

enum DynamicStringResultCode {
    DYNAMIC_STRING_SUCCESS = 0,
    DYNAMIC_STRING_ERROR_OUT_OF_BOUNDS = 1,
    DYNAMIC_STRING_ERROR_MEMORY_ALLOCATION = 2
};

struct DynamicStringResult {
    enum DynamicStringResultCode code;
    char c;
};

struct DynamicString* DynamicString_new(size_t initialSize);
void DynamicString_free(struct DynamicString* self);
struct DynamicStringResult DynamicString_get(struct DynamicString* self, size_t index);
enum DynamicStringErrorCode DynamicString_set(struct DynamicString* self, size_t index, char c);
enum DynamicStringErrorCode DynamicString_add(struct DynamicString* self, char c);

#endif
