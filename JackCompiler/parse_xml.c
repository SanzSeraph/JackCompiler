#include "parse_xml.h"
#include "node.h"
#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>
#include <stdint.h>
#define MAX_NAMESPACE_LENGTH 128
#define MAX_ELEMENT_NAME_LENGTH 1024


bool isLegalElementFirstCharacter(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

bool isLegalElementSubsequentCharacter(char c) 
{
    return isLegalElementFirstCharacter(c) || (c >= '0' && c <= '9') || c == '-' || c == '.' || c == ':';
}

struct ParseResult* ParseResult_new() 
{
    struct ParseResult* result = (struct ParseResult*)malloc(sizeof(struct ParseResult));
    if (result == NULL) {
        return NULL;
    }

    result->node = NULL;
    result->code = PARSE_XML_SUCCESS;
    result->line = 1;
    result->column = 1;
    memset(result->message, 0, sizeof(result->message));

	return result;
}

void ParseResult_free(struct ParseResult* self) 
{
    if (self != NULL) {
        // We don't want to free the node here as it may be used elsewhere
        free(self);
    }
}

struct ParseResult *parseXmlElement(FILE *file, size_t currentLine, size_t currentColumn);
struct KeyValueCollection* parseAttributes(FILE* file, size_t currentLine, size_t currentColumn, struct ParseResult* parseResult);

struct ParseResult parseXml(char* path) 
{
    struct ParseResult result = {
        .node = NULL,
        .code = PARSE_XML_SUCCESS
    };

    result.node = Node_new(NODE_TYPE_DOCUMENT);

    if (result.node == NULL) {
        result.code = PARSE_XML_ERROR_MEMORY_ALLOCATION;
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
            struct ParseResult *rootNodeParseResult = parseXmlElement(file, currentLine, currentColumn, namespaces);

            size_t count = DynamicArray_count(result.node->children);

            if (count > 0) {
                Node_free(result.node);
                Node_free(rootNodeParseResult->node);
                result.code = PARSE_XML_TOO_MANY_ROOT_ELEMENTS;
                goto ret;
            }

            if (rootNodeParseResult->node->nodeType == NODE_TYPE_ELEMENT) {
                DynamicArray_add(result.node->children, rootNodeParseResult->node);
            }
        }
    }

ret:
    return result;
}

struct ParseResult *parseXmlElement(FILE *file, size_t currentLine, size_t currentColumn, struct KeyValueCollection *namespaces) 
{
    struct ParseResult *result = ParseResult_new();

    char c = fgetc(file);
    
    if (!isLegalElementFirstCharacter(c)) {
        ungetc(c, file);

        result->code = PARSE_XML_INVALID_TAG_CHARACTER;
        sprintf(result->message, "Illegal first character %c at line %lu column %lu", c, currentLine, currentColumn);
        result->line = currentLine;
        result->column = currentColumn;
    
        goto ret;
    }

    char *namespace = calloc(MAX_NAMESPACE_LENGTH, sizeof(char));

    if (namespace == NULL) {
        result->code = PARSE_XML_ERROR_MEMORY_ALLOCATION;
        sprintf(result->message, "Failed to allocate dynamicNamespace");

        goto ret;
    }

    char *elementName = calloc(MAX_ELEMENT_NAME_LENGTH, sizeof(char));

    if (elementName == NULL) {
        result->code = PARSE_XML_ERROR_MEMORY_ALLOCATION;
        sprintf(result->message, "Failed to allocate dynamicElemetnName");

        goto ret;
    }

    short index = 0;

    while ((c = fgetc(file)) != EOF && c != ' ' && c != '\n') {
        if (index >= MAX_ELEMENT_NAME_LENGTH) {
            ungetc(c, file);

            result->code = PARSE_XML_ELEMENT_NAME_TOO_LONG;
            sprintf(result->message, "Element name starting with %s is too long.", elementName);
            result->line = currentLine;
            result->column = currentColumn;
            currentColumn--;

            goto ret;
        }

        if (!isLegalElementSubsequentCharacter(c)) {
            ungetc(c, file);

            result->code = PARSE_XML_INVALID_TAG_CHARACTER;
            sprintf(result->message, "Illegal character %c at line %lu column %lu", c, currentLine, currentColumn);
            result->line = currentLine;
            result->column = currentColumn;
            currentColumn--;

            goto ret;
        }

        if (c == ':') {
            memcpy(namespace, elementName, sizeof(namespace));
            memset(elementName, 0, sizeof(elementName));
        } else {
            elementName[index] = c;
        }

        currentColumn++;
    }

    result->node = ElementNode_new(namespace, elementName);
    ((struct ElementNode*)result->node)->attributes = parseAttributes(file, currentLine, currentColumn, result);

    if (result->code != PARSE_XML_SUCCESS) {
        goto ret;
    }

    ret:
        return result;
}

struct KeyValueCollection* parseAttributes(FILE* file, size_t currentLine, size_t currentColumn, struct ParseResult* parseResult)
{
    char c;

    while ((c = fgetc(file)) != '>') {

        currentColumn++;

        while ((c = fgetc(file)) == ' ' || c == '\n' || c == '\t') {
            currentColumn++;
        }
    }
}