#include <stdlib.h>
#include "node.h"
#include "..\lib\dynamic_array.h"

struct Node *Node_new(enum NodeType nodeType) {
    struct Node *self = (struct Node *)malloc(sizeof(struct Node));
    
    if (self == NULL) {
        goto ret;
    }

    *self = (struct Node) {
        .parent = NULL,
        .children = NULL,
        .nodeType = nodeType
    };

    ret:
    return self;
}

void Node_free(struct Node *self) {
    if (self == NULL) {
        goto ret;
    }

    if (self->children != NULL) {
        size_t childCount = DynamicArray_size(self->children);

        for (size_t i = 0; i < childCount; i++) {
            struct DynamicArrayResult result = DynamicArray_get(self->children, i);

            if (result.code == DYNAMIC_ARRAY_SUCCESS) {
                if (result.data != NULL) {
                    Node_free((struct Node *)result.data);
                }
            }
        }

        DynamicArray_free(self->children);
    }

    free(self);

    ret:
    return;
}

enum NodeReturnCodes Node_addChild(struct Node *self, struct Node *child) {
    enum NodeReturnCodes code = NODE_SUCCESS;

    if (self->children == NULL) {
        self->children = DynamicArray_new(10);

        if (self->children == NULL) {
            code = NODE_ERROR_MEMORY_ALLOCATION;
            goto ret;
        }
    }

    enum DynamicArrayErrorCode innerCode = DynamicArray_add(self->children, child);

    if (innerCode == DYNAMIC_ARRAY_ERROR_MEMORY_ALLOCATION) {
        code = NODE_ERROR_MEMORY_ALLOCATION;
        goto ret;
    }

    child->parent = self;

    ret:
    return code;
}



struct ElementNode *ElementNode_new(char *ns, char *nodeName) {
    struct ElementNode *self = (struct ElementNode *)malloc(sizeof(struct ElementNode));

    if (self == NULL) {
        goto ret;
    }

    *self = (struct ElementNode) {
        .base = (struct Node) {
            .children = NULL,
            .nodeType = NODE_TYPE_ELEMENT,
            .parent = NULL
        },
        .ns = ns,
        .nodeName = nodeName,
        .attributes = NULL,
    };

    ret:
    return self;
}

void ElementNode_free(struct ElementNode *self) {
    if (self == NULL) {
        goto ret;
    }

    free(self->nodeName);
    Node_free((struct Node *)self);
    
    ret:
    return;
}

enum NodeReturnCodes ElementNode_setAttribute(struct ElementNode *self, char *name, char *value) {
    enum NodeReturnCodes code = NODE_SUCCESS;

    if (self->attributes == NULL) {
        self->attributes = KeyValueCollection_new();

        if (self->attributes == NULL) {
            code = NODE_ERROR_MEMORY_ALLOCATION;
            goto ret;
        }
    }

    enum KeyValueCollectionErrorCode innerCode = KeyValueCollection_set(self->attributes, name, value);

    if (innerCode == KEY_VALUE_COLLECTION_ERROR_MEMORY_ALLOCATION) {
        code = NODE_ERROR_MEMORY_ALLOCATION;
        goto ret;
    }

    ret:
    return code;
}

struct KeyValue *ElementNode_getAttribute(struct ElementNode *self, char *name) {
    struct KeyValue *attribute = NULL;

    if (self->attributes != NULL) {
        attribute = KeyValueCollection_get(self->attributes, name);
    }

    return attribute;
}

struct TextNode *TextNode_new(char *value) {
    struct TextNode *self = (struct TextNode *)malloc(sizeof(struct TextNode));

    if (self == NULL) {
        goto ret;
    }

    *self = (struct TextNode) {
        .base = (struct Node) {
            .children = NULL,
            .nodeType = NODE_TYPE_TEXT,
            .parent = NULL
        },
        .value = value
    };

    ret:
    return self;
}

void TextNode_free(struct TextNode *self);