#include "parse_xml.h"
#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>

wchar_t legalStartCharacters[] = L"_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
wchar_t legalSubsequentCharacters[] = L".-0123456789_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

struct Node *parseXml(wchar_t (*getWideCharacter)(), wchar_t (*ungetWideCharacter)()) {
    struct Node *document = Node_new(NULL, NODE_TYPE_DOCUMENT);

    if (document == NULL) {
        goto ret;
    }

    char currentChar;
    
    while ((currentChar = getWideCharacter()) != EOF) {
        if (currentChar == L'<') {
            struct Node *node = parseXmlElement(getWideCharacter, ungetWideCharacter);

            if (node->nodeType == NODE_TYPE_ELEMENT) {
                DynamicArray_add(document->children, node);
            }
        } 
    }

    ret:
    return document;
}

struct ElementNode *parseXmlElement(wchar_t (*getWideCharacter)(), wchar_t (*ungetWideCharacter)()) {
    struct ElementNode *element = ElementNode_new(NULL, NULL);

    if (element == NULL) {
        goto ret;
    }



    ret:
    return element;
}