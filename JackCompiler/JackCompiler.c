#include "dynamic_array.h"
#include "dynamic_string.h"
#include "key_value_collection.h"
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool testDynamicArray(void);
bool testDynamicString(void);
bool testAttributeCollection(void);

int main(int argc, char* argv[]) {
    bool success = testDynamicArray();
    success = success && testAttributeCollection();

    if (success) {
        return 0;
    }
}

bool testDynamicArray(void) {
    puts("Testing DynamicArray with strings");

    struct DynamicArray* da = DynamicArray_new(10);
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
    
    result = DynamicArray_get(da, 11);

    assert(result.code == DYNAMIC_ARRAY_SUCCESS);
    assert(strncmp("twelfth", (const char*)result.data, 100) == 0);

    DynamicArray_free(da);

    puts("DynamicArray tests passed successfully!");

    return true;
}

bool testAttributeCollection(void) {
    puts("Testing KeyValueCollection");

    struct KeyValueCollection* keyValueCollection = KeyValuePairCollection_new();
    assert(keyValueCollection != NULL);

    enum KeyValueCollectionErrorCode code = KeyValuePairCollection_set(keyValueCollection, "name", "Caitlyn");

    assert(code == KEY_VALUE_COLLECTION_SUCCESS);

    struct KeyValuePair* kv = KeyValuePairCollection_get(keyValueCollection, "name");

    assert(kv != NULL);
    assert(strncmp(kv->value, "Caitlyn", 100) == 0);

    code = KeyValuePairCollection_set(keyValueCollection, "age", "29");

    assert(code == KEY_VALUE_COLLECTION_SUCCESS);

    kv = KeyValuePairCollection_get(keyValueCollection, "age");

    assert(kv != NULL);
    assert(strncmp(kv->value, "29", 100) == 0);

    kv = KeyValuePairCollection_get(keyValueCollection, "nonexistent");

    assert(kv == NULL);

    code = KeyValuePairCollection_set(keyValueCollection, "name", "Not Caitlyn");

    assert(code == KEY_VALUE_COLLECTION_SUCCESS);

    kv = KeyValuePairCollection_get(keyValueCollection, "name");

    assert(kv != NULL);
    assert(strncmp(kv->value, "Not Caitlyn", 100) == 0);

    KeyValuePairCollection_free(keyValueCollection);

    puts("KeyValueCollection tests passed successfully!");

    return true;
}

bool testDynamicString(void) {
    puts("Testing DynamicString");

	struct DynamicString* ds = DynamicString_new(5);

    DynamicString_add(ds, 'H');
	DynamicString_add(ds, 'e');
	DynamicString_add(ds, 'l');
	DynamicString_add(ds, 'l');
	DynamicString_add(ds, 'o');
	DynamicString_add(ds, ' ');
	DynamicString_add(ds, 'W');
	DynamicString_add(ds, 'o');
	DynamicString_add(ds, 'r');
	DynamicString_add(ds, 'l');
	DynamicString_add(ds, 'd');
	DynamicString_add(ds, '!');
	DynamicString_add(ds, '\0');

    puts(ds->array);

    assert(strncmp(ds->array, "Hello World!", ds->capacity));

    puts("DynamicString tests passed successfully!");
}