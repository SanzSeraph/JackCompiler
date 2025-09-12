#include "parse_xml.h"
#include "node.h"
#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>
#include <stdint.h>
#define MAX_NAMESPACE_LENGTH 128
#define MAX_ELEMENT_NAME_LENGTH 1024
#define MAX_ATTRIBUTE_NAME_LENGTH 256
#define MAX_ATTRIBUTE_VALUE_LENGTH 2048

bool isLegalElementFirstCharacter(char c);
bool isLegalElementSubsequentCharacter(char c);
bool isWhitespace(char c);

struct ParseResult parseXmlElement(FILE *file, size_t currentLine, size_t currentColumn);
struct ParseResult parseAttributes(FILE* file, size_t currentLine, size_t currentColumn);
struct ParseResult consumeWhitespace(FILE* file, size_t currentLine, size_t currentColumn);
struct ParseResult consumeProlog(FILE* file, size_t currentLine, size_t currentColumn);

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
    
	consumeWhitespace(file, &currentLine, &currentColumn);

    char currentChar;
    struct KeyValueCollection* namespaces = KeyValueCollection_new();

    while ((currentChar = fgetc(file)) != EOF) {
        if (currentChar == '<') {
            currentColumn++;

            currentChar = fgetc(file);
			currentColumn++;

            if (currentChar == '?') {
                struct ParseResult prologParseResult = consumeProlog(file, &currentLine, &currentColumn);

                if (prologParseResult.code != PARSE_XML_SUCCESS) {
                    result.code = prologParseResult.code;

                    goto ret;
				}
            } else {
                ungetc(currentChar, file);
				currentColumn--;
            }

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

/// <summary>
/// Assumes that the < has already been consumed
/// </summary>
/// <param name="file"></param>
/// <param name="currentLine"></param>
/// <param name="currentColumn"></param>
/// <param name="namespaces"></param>
/// <returns></returns>
struct ParseResult parseXmlElement(FILE *file, size_t currentLine, size_t currentColumn, struct KeyValueCollection *namespaces) 
{
    struct ParseResult result = (struct ParseResult){
        .code = PARSE_XML_SUCCESS,
        .value = NULL
    };

    char currentChar = fgetc(file);
    
    if (!isLegalElementFirstCharacter(currentChar)) {
        ungetc(currentChar, file);

        result.code = PARSE_XML_INVALID_TAG_CHARACTER;
        sprintf(result.message, "Illegal first character %c at line %lu column %lu", currentChar, currentLine, currentColumn);
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

    while ((currentChar = fgetc(file)) != EOF && !isWhitespace(currentChar)) {
        currentColumn++;

        if (index >= MAX_ELEMENT_NAME_LENGTH) {
            result.code = PARSE_XML_ELEMENT_NAME_TOO_LONG;
            sprintf(result.message, "Element name starting with %s is too long.", elementName);
            result.line = currentLine;
            result.column = currentColumn;

            goto ret;
        }

        if (!isLegalElementSubsequentCharacter(currentChar)) {
            result.code = PARSE_XML_INVALID_TAG_CHARACTER;
            sprintf(result.message, "Illegal character %c at line %lu column %lu", currentChar, currentLine, currentColumn);
            result.line = currentLine;
            result.column = currentColumn;

            goto ret;
        }

        if (currentChar == ':') {
            memcpy(namespace, elementName, sizeof(namespace));
            memset(elementName, 0, sizeof(elementName));
        } else {
            elementName[index] = currentChar;
        }
    }

    if (currentChar == EOF) {
        result.code = PARSE_XML_UNEXPECTED_EOF;
        sprintf(result.message, "Unexpected end of file while parsing element %s:%s", namespace, elementName);
        result.column = currentColumn;
        result.line = currentLine;

        goto ret;
    } 
    
    if (currentChar == '\n') {
        currentColumn = 1;
        currentLine++;
    }

	struct ParseResult consumeWhitespaceResult = consumeWhitespace(file, currentLine, currentColumn);

    result.value = ElementNode_new(namespace, elementName);

    struct ElementNode* elementNode = (struct ElementNode*)result.value;

    struct ParseResult parseAttributesResult = parseAttributes(file, &currentLine, &currentColumn);

    if (parseAttributesResult.code != PARSE_XML_SUCCESS) {
        result.code == parseAttributesResult.code;
        result.line = parseAttributesResult.line;
		result.column = parseAttributesResult.column;
        sprintf(result.message, parseAttributesResult.message);
		
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

    char currentChar;
    char key[MAX_ATTRIBUTE_NAME_LENGTH];
	char value[MAX_ATTRIBUTE_NAME_LENGTH];

    while ((currentChar = fgetc(file))) {

    }

    ret: 
    return result;
}

struct ParseResult consumeWhitespace(FILE *file, size_t currentLine, size_t currentColumn) 
{
    struct ParseResult result = {
        .code = PARSE_XML_SUCCESS,
        .value = NULL
	};

    char currentChar;

    while((currentChar = fgetc(file)) != EOF && (currentChar == ' ' || currentChar == '\t' || currentChar == '\n')) {
        if (currentChar == '\n') {
            currentLine++;
            currentColumn = 0;
        } else {
            currentColumn++;
        }
	}

    if (currentChar == EOF) {
		result.code = PARSE_XML_UNEXPECTED_EOF;
        result.line = currentLine;
		result.column = currentColumn;
        sprintf(result.message, "Unexpected end of file");
        
        goto ret;
    }

	ungetc(currentChar, file);
    currentColumn--;
    result.line = currentLine;
	result.column = currentColumn;
    
    ret:
        return result;
}

/// <summary>
/// Assumes that the <? part of the prolog have already been consumed
/// </summary>
struct ParseResult consumeProlog(FILE* file, size_t currentLine, size_t currentColumn) 
{
    struct ParseResult result = {
        .code = PARSE_XML_SUCCESS,
        .value = NULL
	};

    char currentChar = fgetc(file);
    currentColumn++;

    if (currentChar == EOF) {
		result.code = PARSE_XML_UNEXPECTED_EOF;
		result.line = currentLine;
		result.column = currentColumn;

        goto ret;
    }
	
    if (currentChar != 'x') {
		result.code = PARSE_XML_INVALID_TAG_CHARACTER;
        result.line = currentLine;
		result.column = currentColumn;
		sprintf(result.message, "Expected 'xml' at line %lu column %lu", currentLine, currentColumn);

        goto ret;
    }

	currentChar = fgetc(file);
	currentColumn++;

    if (currentChar != 'm') {
        result.code = PARSE_XML_INVALID_TAG_CHARACTER;
		result.line = currentLine;
        result.column = currentColumn;
		sprintf(result.message, "Expected 'xml' at line %lu column %lu", currentLine, currentColumn);

		goto ret;
    }

	currentChar = fgetc(file);
    currentColumn++;

    if (currentChar != 'l') {
        result.code = PARSE_XML_INVALID_TAG_CHARACTER;
		result.line = currentLine;
		result.column = currentColumn;
		sprintf(result.message, "Expected 'xml' at line %lu column %lu", currentLine, currentColumn);

        goto ret;
    }

	currentChar = fgetc(file);
	currentColumn++;

    if (currentChar != ' ') {
        result.code = PARSE_XML_INVALID_TAG_CHARACTER;
		result.line = currentLine;
		result.column = currentColumn;
		sprintf(result.message, "Expected whitespace after 'xml' at line %lu column %lu", currentLine, currentColumn);

        goto ret;
    }

    bool inString = false;
    
    while ((currentChar = fgetc(file)) != EOF) {
        if (currentChar = '\n') {
            currentLine++;
            currentColumn = 0;
        }
        else {
            currentColumn++;
        }
        		
        if (currentChar == '"') {
            inString = !inString;
        }
        else if (currentChar == '?' && !inString) {
            currentChar = fgetc(file);
            currentColumn++;
            if (currentChar != '>') {
                result.code = PARSE_XML_INVALID_TAG_CHARACTER;
                result.line = currentLine;
                result.column = currentColumn;
                sprintf(result.message, "Expected > to close the prolog at line %lu column %lu", currentLine, currentColumn);

                goto ret;
            }
            else {
                goto ret;
            }
        }
    }

    ret:
	return result;
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