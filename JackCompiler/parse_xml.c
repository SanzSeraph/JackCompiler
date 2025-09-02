#include "parse_xml.h"
#include "node.h"
#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>
#include <stdint.h>

bool isLegalElementFirstCharacter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

bool isLegalElementSubsequentCharacter(char c) {
    return isLegalElementFirstCharacter(c) || (c >= '0' && c <= '9') || c == '-' || c == '.' || c == ':';
}

struct ParseResult* ParseResult_new() {
    struct ParseResult* result = (struct ParseResult*)malloc(sizeof(struct ParseResult));
    if (result == NULL) {
        return NULL;
    }
    result->node = NULL;
    result->code = PARSE_XML_SUCCESS;
    result->line = 1;
    result->column = 1;
    result->message = NULL;

	return result;
}

void ParseResult_free(struct ParseResult* self) {
    if (self != NULL) {
        if (self->message != NULL) {
            free(self->message);
        }
        // We don't want to free the node here as it may be used elsewhere
        free(self);
    }
}

struct ParseResult *parseXmlElement(FILE* file, size_t currentLine, size_t currentColumn);

struct ParseResult parseXml(char* path) {
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

    wchar_t currentChar;

    while ((currentChar = fgetc(file)) != EOF) {
        if (currentChar == '<') {
            struct ParseResult rootNodeParseResult = parseXmlElement(file);

            size_t count = DynamicArray_count(result.node->children);

            if (count > 0) {
                Node_free(result.node);
                Node_free(rootNodeParseResult.node);
                result.code = PARSE_XML_TOO_MANY_ROOT_ELEMENTS;
                goto ret;
            }

            if (rootNodeParseResult.node->nodeType == NODE_TYPE_ELEMENT) {
                DynamicArray_add(result.node->children, rootNodeParseResult.node);
            }
        }
    }

ret:
    return result;
}

struct ParseResult *parseXmlElement(FILE *file, size_t currentLine, size_t currentColumn) {
    struct ParseResult *result = 
    struct ParseResult result = {
        .node = NULL,
        .code = PARSE_XML_SUCCESS
    };

    char c = fgetc(file);
    
    if (!isLegalElementFirstCharacter(c)) {
        result.code = PARSE_XML_INVALID_TAG_CHARACTER;
        result.message = format
        goto ret;
    }

    wchar_t elementName[MAX_ELEMENT_NAME_LENGTH];

ret:
    return result;
}