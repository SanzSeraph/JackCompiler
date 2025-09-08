#include "parse_xml.h"
#include "node.h"
#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>
#include <stdint.h>
#define MAX_NAMESPACE_LENGTH 128
#define MAX_ELEMENT_NAME_LENGTH 1024

bool isLegalElementFirstCharacter(char c);
bool isLegalElementSubsequentCharacter(char c);
bool isWhitespace(char c);

struct ParseResult parseXmlElement(FILE *file, size_t currentLine, size_t currentColumn);
struct ParseResult parseAttributes(FILE* file, size_t currentLine, size_t currentColumn);
struct ParseResult consumeWhitespace(FILE* file, size_t* currentLine, size_t* currentColumn);

struct ParseResult parseXml(char* path) 
{
    struct ParseResult result = {
        .value = NULL,
        .code = PARSE_XML_SUCCESS
    };

    result.value = Node_new(NODE_TYPE_DOCUMENT);

    if (result.value == NULL) {
        result.code = PARSE_XML_ERROR_MEMORY_ALLOCATION;
        sprintf(result.message, "Out of memory");

        goto ret;
    }

	FILE* file = fopen(path, "r");

    size_t currentLine = 1;
	size_t currentColumn = 0;
    
    char currentChar;
    struct KeyValueCollection* namespaces = KeyValueCollection_new();

    while ((currentChar = fgetc(file)) != EOF) {
        if (currentChar == '<') {
            currentColumn++;
            struct ParseResult rootNodeParseResult = parseXmlElement(file, currentLine, currentColumn, namespaces);

            size_t count = DynamicArray_count(((struct Node *)result.value)->children);

            if (count > 0) {
                Node_free(result.value);
                Node_free(rootNodeParseResult.value);
                result.code = PARSE_XML_TOO_MANY_ROOT_ELEMENTS;
                goto ret;
            }

            struct Node* rootNode = (struct Node*)rootNodeParseResult.value;

            if (rootNode->nodeType == NODE_TYPE_ELEMENT) {
                struct Node* node = (struct Node*)result.value;

                DynamicArray_add(node->children, rootNode);
            }
        }
    }

ret:
    return result;
}

struct ParseResult parseXmlElement(FILE *file, size_t currentLine, size_t currentColumn, struct KeyValueCollection *namespaces) 
{
    struct ParseResult result = (struct ParseResult){
        .code = PARSE_XML_SUCCESS,
        .value = NULL
    };

    char c = fgetc(file);
    
    if (!isLegalElementFirstCharacter(c)) {
        ungetc(c, file);

        result.code = PARSE_XML_INVALID_TAG_CHARACTER;
        sprintf(result.message, "Illegal first character %c at line %lu column %lu", c, currentLine, currentColumn);
        result.line = currentLine;
        result.column = currentColumn;
    
        goto ret;
    }

    char *namespace = calloc(MAX_NAMESPACE_LENGTH, sizeof(char));

    if (namespace == NULL) {
        result.code = PARSE_XML_ERROR_MEMORY_ALLOCATION;
        sprintf(result.message, "Failed to allocate dynamicNamespace");

        goto ret;
    }

    char *elementName = calloc(MAX_ELEMENT_NAME_LENGTH, sizeof(char));

    if (elementName == NULL) {
        result.code = PARSE_XML_ERROR_MEMORY_ALLOCATION;
        sprintf(result.message, "Failed to allocate dynamicElemetnName");

        goto ret;
    }

    short index = 0;

    while ((c = fgetc(file)) != EOF && !isWhitespace(c)) {
        currentColumn++;

        if (index >= MAX_ELEMENT_NAME_LENGTH) {
            result.code = PARSE_XML_ELEMENT_NAME_TOO_LONG;
            sprintf(result.message, "Element name starting with %s is too long.", elementName);
            result.line = currentLine;
            result.column = currentColumn;

            goto ret;
        }

        if (!isLegalElementSubsequentCharacter(c)) {
            result.code = PARSE_XML_INVALID_TAG_CHARACTER;
            sprintf(result.message, "Illegal character %c at line %lu column %lu", c, currentLine, currentColumn);
            result.line = currentLine;
            result.column = currentColumn;

            goto ret;
        }

        if (c == ':') {
            memcpy(namespace, elementName, sizeof(namespace));
            memset(elementName, 0, sizeof(elementName));
        } else {
            elementName[index] = c;
        }
    }

    if (c == EOF) {
        result.code = PARSE_XML_UNEXPECTED_EOF;
        sprintf(result.message, "Unexpected end of file while parsing element %s:%s", namespace, elementName);
        result.column = currentColumn;
        result.line = currentLine;

        goto ret;
    } if (c == ' ' || c == '\t') {
        currentColumn++;
    } else if (c == '\n') {
        currentColumn = 1;
        currentLine++;
    }

    result.value = ElementNode_new(namespace, elementName);

    struct ElementNode* elementNode = (struct ElementNode*)result.value;

    struct ParseResult parseAttributesResult = parseAttributes(file, &currentLine, &currentColumn);

    if (parseAttributesResult.code != PARSE_XML_SUCCESS) {
        result = parseAttributesResult;

        goto ret;
    }

    elementNode->attributes = parseAttributesResult.value;

    ret:
        return result;
}

struct ParseResult parseAttributes(FILE* file, size_t *currentLine, size_t *currentColumn)
{
    struct ParseResult result = (struct ParseResult){
        .code = PARSE_XML_SUCCESS,
        .value = NULL
    };

    struct ParseResult consumeWhitespaceResult = consumeWhitespace(file, currentLine, currentColumn);

    if (consumeWhitespaceResult.code != PARSE_XML_SUCCESS) {
        result = consumeWhitespaceResult;

        goto ret;
    }

    char c;

    ret: 
    return result;
}

struct ParseResult consumeWhitespace(FILE *file, size_t *currentLine, size_t *currentColumn) 
{
    
}

bool isLegalElementFirstCharacter(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

bool isLegalElementSubsequentCharacter(char c)
{
    return isLegalElementFirstCharacter(c) || (c >= '0' && c <= '9') || c == '-' || c == '.' || c == ':';
}

bool isWhitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n';
}