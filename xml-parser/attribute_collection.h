#ifndef ATTRIBUTE_COLLECTION_H
#define ATTRIBUTE_COLLECTION_H
#include <stdlib.h>
#include "..\lib\dynamic_array.h"

struct AttributeCollection {
    struct DynamicArray *attributes;
};

struct Attribute {
    char *name;
    char *value;
};

enum AttributeCollectionErrorCode {
    ATTRIBUTE_COLLECTION_SUCCESS = 0,
    ATTRIBUTE_COLLECTION_ERROR_MEMORY_ALLOCATION = 1
};

struct AttributeCollection *AttributeCollection_new();
void AttributeCollection_free(struct AttributeCollection *self);
struct Attribute *AttributeCollection_get(struct AttributeCollection *self, char *name);
enum AttributeCollectionErrorCode AttributeCollection_set(struct AttributeCollection *self, char *name, char *value);

#endif // ATTRIBUTE_COLLECTION_H