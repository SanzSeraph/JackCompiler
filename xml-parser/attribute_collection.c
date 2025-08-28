#include "attribute_collection.h"

struct AttributeCollection *AttributeCollection_new()
{
    struct AttributeCollection *collection = (struct AttributeCollection *)malloc(sizeof(struct AttributeCollection));
    
    if (collection == NULL) {
        goto ret;
    }

    *collection = (struct AttributeCollection){
        .attributes = DynamicArray_new(10)
    };

    ret:
    return collection;
}

void AttributeCollection_free(struct AttributeCollection *self) 
{
    if (self == NULL) {
        goto ret;
    }

    size_t size = DynamicArray_size(self->attributes);
    
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

struct Attribute *AttributeCollection_get(struct AttributeCollection *self, char *name) 
{
    size_t size = DynamicArray_size(self->attributes);
    struct Attribute *attribute = NULL;

    for (size_t i = 0; i < size; i++) {
        struct DynamicArrayResult result = DynamicArray_get(self->attributes, i);

        if (result.code == DYNAMIC_ARRAY_SUCCESS && result.data != NULL) {
            struct Attribute *currentAttribute = (struct Attribute *)result.data;

            if (strcmp(currentAttribute->name, name) == 0) {
                attribute = currentAttribute;
                goto ret;
            }
        }
    }

    ret:
    return attribute;
}

enum AttributeCollectionErrorCode AttributeCollection_set(struct AttributeCollection *self, char *name, char *value) 
{
    enum AttributeCollectionErrorCode code = ATTRIBUTE_COLLECTION_SUCCESS;
    struct Attribute *attribute = AttributeCollection_get(self, name);

    if (attribute == NULL) {
        attribute = (struct Attribute *)malloc(sizeof(struct Attribute));

        if (attribute == NULL) {
            code = ATTRIBUTE_COLLECTION_ERROR_MEMORY_ALLOCATION;
            goto ret;
        }

        attribute->name = name;

        enum DynamicArrayErrorCode innerCode = DynamicArray_add(self->attributes, attribute);

        if (innerCode == DYNAMIC_ARRAY_ERROR_MEMORY_ALLOCATION) {
            code = ATTRIBUTE_COLLECTION_ERROR_MEMORY_ALLOCATION;
        }
    }

    attribute->value = value;
    
    ret:
    return code;
}