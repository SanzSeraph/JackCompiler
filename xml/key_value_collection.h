#ifndef KEY_VALUE_COLLECTION_H
#define KEY_VALUE_COLLECTION_H
#include <stdlib.h>
#include "..\lib\dynamic_array.h"

struct KeyValueCollection {
    struct DynamicArray *attributes;
};

struct KeyValue {
    char *name;
    char *value;
};

enum KeyValueCollectionErrorCode {
    KEY_VALUE_COLLECTION_SUCCESS = 0,
    KEY_VALUE_COLLECTION_ERROR_MEMORY_ALLOCATION = 1
};

struct KeyValueCollection *KeyValueCollection_new();
void KeyValueCollection_free(struct KeyValueCollection *self);
struct KeyValue *KeyValueCollection_get(struct KeyValueCollection *self, char *name);
enum KeyValueCollectionErrorCode KeyValueCollection_set(struct KeyValueCollection *self, char *name, char *value);

#endif // KEY_VALUE_COLLECTION_H