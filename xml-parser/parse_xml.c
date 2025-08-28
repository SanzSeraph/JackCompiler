#include "parse_xml.h"



struct Node *parseXml(char (*getNextCharacter)()) {
    struct Node *document = Node_new(NULL, NODE_TYPE_DOCUMENT);

    if (document == NULL) {
        goto ret;
    }

    

    ret:
    return document;
}