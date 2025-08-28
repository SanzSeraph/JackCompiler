#ifndef NODE_H
#define NODE_H
#include "attribute_collection.h"

enum NodeType {
    NODE_TYPE_DOCUMENT,
    NODE_TYPE_DOCUMENT_TYPE,
    NODE_TYPE_ELEMENT,
    NODE_TYPE_TEXT,
    NODE_TYPE_COMMENT,
    NODE_TYPE_CDATA_SECTION
};

struct Node {
    struct Node *parent;
    struct Node **children;
    struct AttributeCollection *attributes;
    char *ns;
    enum NodeType nodeType;
};

struct ElementNode {
    struct Node base;
    char *nodeName;
};

struct TextNode {
    struct Node base;
    char *value;
};

enum NodeReturnCodes {
    NODE_SUCCESS = 0,
    NODE_ERROR_MEMORY_ALLOCATION = 1
};

struct Node *Node_new(char *ns, enum NodeType nodeType);
void Node_free(struct Node *self);
enum NodeReturnCodes Node_addChild(struct Node *self, struct Node *child);
enum NodeReturnCodes Node_setAttribute(struct Node *self, char *name, char *value);
struct Attribute *Node_getAttribute(struct Node *self, char *name);

struct ElementNode *ElementNode_new(char *ns, char *nodeName);
void ElementNode_free(struct ElementNode *self);

struct TextNode *TextNode_new(char *ns, char *value);
void TextNode_free(struct TextNode *self);

#endif // NODE_H