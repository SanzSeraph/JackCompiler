#ifndef NODE_H
#define NODE_H
#include "key_value_collection.h"
#include "dynamic_array.h"

enum NodeType {
    NODE_TYPE_DOCUMENT,
    NODE_TYPE_DOCUMENT_TYPE,
    NODE_TYPE_ELEMENT,
    NODE_TYPE_TEXT,
    NODE_TYPE_COMMENT,
    NODE_TYPE_CDATA_SECTION
};

struct Node {
    struct Node* parent;
    struct DynamicArray* children;
    enum NodeType nodeType;
};

struct ElementNode {
    struct Node base;
    char* ns;
    char* nodeName;
    struct KeyValueCollection* attributes;
    struct KeyValueCollection* dataset;
};

struct TextNode {
    struct Node base;
    char* value;
};

enum NodeReturnCodes {
    NODE_SUCCESS = 0,
    NODE_ERROR_MEMORY_ALLOCATION = 1
};

struct Node* Node_new(enum NodeType nodeType);
void Node_free(struct Node* self);
enum NodeReturnCodes Node_addChild(struct Node* self, struct Node* child);

struct ElementNode* ElementNode_new(char* ns, char* nodeName);
void ElementNode_free(struct ElementNode* self);
enum NodeReturnCodes ElementNode_setAttribute(struct ElementNode* self, char* name, char* value);
struct KeyValue* ElementNode_getAttribute(struct ElementNode* self, char* name);

struct TextNode* TextNode_new(char* value);
void TextNode_free(struct TextNode* self);

#endif // NODE_H