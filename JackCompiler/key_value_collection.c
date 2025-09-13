#include "key_value_collection.h"

struct KeyValueCollection* KeyValueCollection_new()
{
    struct KeyValueCollection* collection = (struct KeyValueCollection*)malloc(sizeof(struct KeyValueCollection));

    if (collection == NULL) {
        goto ret;
    }

    *collection = (struct KeyValueCollection){
        .attributes = DynamicArray_new(10)
    };

ret:
    return collection;
}

void KeyValueCollection_free(struct KeyValueCollection* self)
{
    if (self == NULL) {
        goto ret;
    }

    size_t size = self->attributes->currentEnd;

    for (size_t i = 0; i < size; i++) {
        struct DynamicArrayResult result = DynamicArray_get(self->attributes, i);

        if (result.code == DYNAMIC_ARRAY_SUCCESS) {
            if (result.data != NULL) {
                free(result.data);
            }
        }
    }

    DynamicArray_free(self->attributes);
    free(self);

ret:
    return;
}

struct KeyValue* KeyValueCollection_get(struct KeyValueCollection* self, char* name)
{
    size_t size = self->attributes->currentEnd;
    struct KeyValue* kvp = NULL;

    for (size_t i = 0; i < size; i++) {
        struct DynamicArrayResult result = DynamicArray_get(self->attributes, i);

        if (result.code == DYNAMIC_ARRAY_SUCCESS && result.data != NULL) {
            struct KeyValue* currentAttribute = (struct KeyValue*)result.data;

            if (strcmp(currentAttribute->name, name) == 0) {
                kvp = currentAttribute;
                goto ret;
            }
        }
    }

ret:
    return kvp;
}

enum KeyValueCollectionErrorCode KeyValueCollection_set(struct KeyValueCollection* self, char* name, char* value)
{
    enum KeyValueCollectionErrorCode code = KEY_VALUE_COLLECTION_SUCCESS;
    struct KeyValue* attribute = KeyValueCollection_get(self, name);

    if (attribute == NULL) {
        attribute = (struct KeyValue*)malloc(sizeof(struct KeyValue));

        if (attribute == NULL) {
            code = KEY_VALUE_COLLECTION_ERROR_MEMORY_ALLOCATION;
            goto ret;
        }

        attribute->name = name;

        enum DynamicArrayErrorCode innerCode = DynamicArray_add(self->attributes, attribute);

        if (innerCode == DYNAMIC_ARRAY_ERROR_MEMORY_ALLOCATION) {
            code = KEY_VALUE_COLLECTION_ERROR_MEMORY_ALLOCATION;
        }
    }

    attribute->value = value;

ret:
    return code;
}