#include "xml-parser/dynamic_array.h"
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool testDynamicArray(void);

int main(int argc, char *argv[])
{
    bool success = testDynamicArray();
    
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