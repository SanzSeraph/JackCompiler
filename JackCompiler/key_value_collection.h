#ifndef KEY_VALUE_COLLECTION_H
#define KEY_VALUE_COLLECTION_H
#include <stdlib.h>
#include "dynamic_array.h"

struct KeyValueCollection {
    struct DynamicArray* items;
};

struct KeyValuePair {
    struct DynamicString *key;
    struct DynamicString *value;
};

enum KeyValueCollectionErrorCode {
    KEY_VALUE_COLLECTION_SUCCESS = 0,
    KEY_VALUE_COLLECTION_ERROR_MEMORY_ALLOCATION = 1
};

struct KeyValuePair* KeyValue_new();
void KeyValue_free(struct KeyValuePair* self);

struct KeyValueCollection* KeyValuePairCollection_new();
void KeyValuePairCollection_free(struct KeyValueCollection* self);
struct KeyValuePair* KeyValuePairCollection_get(struct KeyValueCollection* self, char* name);
enum KeyValueCollectionErrorCode KeyValuePairCollection_set(struct KeyValueCollection* self, char* name, char* value);

#endif // KEY_VALUE_COLLECTION_H