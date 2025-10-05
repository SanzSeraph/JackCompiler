#include "key_value_collection.h"

struct KeyValuePair* KeyValue_new()
{
    struct KeyValuePair* kvp = (struct KeyValuePair*)malloc(sizeof(struct KeyValuePair));

    if (kvp == NULL) {
        return NULL;
    }

    *kvp = (struct KeyValuePair){
        .key = NULL,
        .value = NULL
    };

    return kvp;                    
}

void KeyValue_free(struct KeyValuePair* kvp) {
    if (kvp != NULL) {
        if (kvp->key != NULL) {
            DynamicString_free(kvp->key);
        }

        if (kvp->value != NULL) {
            DynamicString_freeE(kvp->value);
        }

        free(kvp);
    }
}

struct KeyValueCollection* KeyValuePairCollection_new()
{
    struct KeyValueCollection* collection = (struct KeyValueCollection*)malloc(sizeof(struct KeyValueCollection));

    if (collection == NULL) {
        goto ret;
    }

    *collection = (struct KeyValueCollection){
        .items = DynamicArray_new(10)
    };

ret:
    return collection;
}

void KeyValuePairCollection_free(struct KeyValueCollection* self)
{
    if (self == NULL) {
        goto ret;
    }

    size_t size = self->items->currentEnd;

    for (size_t i = 0; i < size; i++) {
        struct DynamicArrayResult result = DynamicArray_get(self->items, i);

        if (result.code == DYNAMIC_ARRAY_SUCCESS) {
            if (result.data != NULL) {
                free(result.data);
            }
        }
    }

    DynamicArray_free(self->items);
    free(self);

ret:
    return;
}

struct KeyValuePair* KeyValuePairCollection_get(struct KeyValueCollection* self, char* name)
{
    size_t size = self->items->currentEnd;
    struct KeyValuePair* kvp = NULL;

    for (size_t i = 0; i < size; i++) {
        struct DynamicArrayResult result = DynamicArray_get(self->items, i);

        if (result.code == DYNAMIC_ARRAY_SUCCESS && result.data != NULL) {
            struct KeyValuePair* currentAttribute = (struct KeyValuePair*)result.data;

            if (strcmp(currentAttribute->key, name) == 0) {
                kvp = currentAttribute;
                goto ret;
            }
        }
    }

ret:
    return kvp;
}

enum KeyValueCollectionErrorCode KeyValuePairCollection_set(struct KeyValueCollection* self, char* name, char* value)
{
    enum KeyValueCollectionErrorCode code = KEY_VALUE_COLLECTION_SUCCESS;
    struct KeyValuePair* attribute = KeyValuePairCollection_get(self, name);

    if (attribute == NULL) {
        attribute = (struct KeyValuePair*)malloc(sizeof(struct KeyValuePair));

        if (attribute == NULL) {
            code = KEY_VALUE_COLLECTION_ERROR_MEMORY_ALLOCATION;
            goto ret;
        }

        attribute->key = name;

        enum DynamicArrayErrorCode innerCode = DynamicArray_add(self->items, attribute);

        if (innerCode == DYNAMIC_ARRAY_ERROR_MEMORY_ALLOCATION) {
            code = KEY_VALUE_COLLECTION_ERROR_MEMORY_ALLOCATION;
        }
    }

    attribute->value = value;

ret:
    return code;
}