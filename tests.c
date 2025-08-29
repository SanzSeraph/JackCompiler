#include "lib/dynamic_array.h"
#include "xml-parser/attribute_collection.h"
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool testDynamicArray(void);
bool testAttributeCollection(void);

int main(int argc, char *argv[])
{
    bool success = testDynamicArray();
    success = success && testAttributeCollection();
    
    if (success) {
        return 0;
    }
}

bool testDynamicArray(void) {
    puts("Testing DynamicArray with strings");

    struct DynamicArray *da = DynamicArray_new(10);
    assert(da != NULL);

    enum DynamicArrayErrorCode code = DynamicArray_add(da, "Hello World!");

    assert(code == DYNAMIC_ARRAY_SUCCESS);

    code = DynamicArray_add(da, "Hello Caitlyn");

    assert(code == DYNAMIC_ARRAY_SUCCESS);

    struct DynamicArrayResult result = DynamicArray_get(da, 1);

    assert(result.code == DYNAMIC_ARRAY_SUCCESS);

    result = DynamicArray_get(da, 2);

    assert(result.code == DYNAMIC_ARRAY_SUCCESS);
    assert(result.data == NULL);

    result = DynamicArray_get(da, 10);

    assert(result.code == DYNAMIC_ARRAY_ERROR_OUT_OF_BOUNDS);

    code = DynamicArray_set(da, 10, "eleven");

    assert(code == DYNAMIC_ARRAY_ERROR_OUT_OF_BOUNDS);

    code = DynamicArray_add(da, "third");

    assert(code == DYNAMIC_ARRAY_SUCCESS);
    assert(da->currentEnd == 3);

    DynamicArray_add(da, "fourth");
    DynamicArray_add(da, "fifth");
    DynamicArray_add(da, "sixth");
    DynamicArray_add(da, "seventh");
    DynamicArray_add(da, "eigth");
    DynamicArray_add(da, "ninth");
    DynamicArray_add(da, "tenth");
    DynamicArray_add(da, "eleventh");

    code = DynamicArray_add(da, "twelfth");

    assert(code == DYNAMIC_ARRAY_SUCCESS);
    assert(da->partitionCount == 2);

    result = DynamicArray_get(da, 11);

    assert(result.code == DYNAMIC_ARRAY_SUCCESS);
    assert(strncmp("twelfth", (const char *)result.data, 100) == 0);

    DynamicArray_free(da);

    puts("DynamicArray tests passed successfully!");

    return true;
}

bool testAttributeCollection(void) {
    puts("Testing AttributeCollection");

    struct AttributeCollection *ac = KeyValueCollection_new();
    assert(ac != NULL);

    enum AttributeCollectionErrorCode code = KeyValueCollection_set(ac, "name", "Caitlyn");

    assert(code == ATTRIBUTE_COLLECTION_SUCCESS);

    struct Attribute *attr = KeyValueCollection_get(ac, "name");

    assert(attr != NULL);
    assert(strncmp(attr->value, "Caitlyn", 100) == 0);

    code = KeyValueCollection_set(ac, "age", "29");

    assert(code == ATTRIBUTE_COLLECTION_SUCCESS);

    attr = KeyValueCollection_get(ac, "age");

    assert(attr != NULL);
    assert(strncmp(attr->value, "29", 100) == 0);

    attr = KeyValueCollection_get(ac, "nonexistent");

    assert(attr == NULL);

    code = KeyValueCollection_set(ac, "name", "Not Caitlyn");

    assert(code == ATTRIBUTE_COLLECTION_SUCCESS);

    attr = KeyValueCollection_get(ac, "name");

    assert(attr != NULL);
    assert(strncmp(attr->value, "Not Caitlyn", 100) == 0);

    AttributeCollection_free(ac);

    puts("AttributeCollection tests passed successfully!");

    return true;
}