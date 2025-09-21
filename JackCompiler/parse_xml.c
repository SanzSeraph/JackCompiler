#include "parse_xml.h"
#include "node.h"
#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>
#include <stdint.h>
#include <string.h>
#include "attribute_collection.h"


#define OUT_OF_MEMORY_MESSAGE "Out of memory"
#define ILLEGAL_FIRST_CHARACTER_MESSAGE "Illegal first character %c at line %lu column %lu"
#define FAILED_TO_ALLOCATE_NAMESPACE_MESSAGE "Failed to allocate namespace"
#define FAILED_TO_ALLOCATE_ELEMENT_NAME_MESSAGE "Failed to allocate element name"
#define ELEMENT_NAME_TOO_LONG_MESSAGE "Element name starting with %s is too long."
#define ILLEGAL_ELEMENT_NAME_CHARACTER_MESSAGE "Illegal character %c at line %lu column %lu"
#define UNEXPECTED_EOF_MESSAGE "Unexpected end of file"
#define EXPECTED_XML_MESSAGE "Expected 'xml' at line %lu column %lu"
#define EXPECTED_WHITESPACE_AFTER_XML_MESSAGE "Expected whitespace after 'xml' at line %lu column %lu"
#define EXPECTED_LT_TO_CLOSE_PROLOG_MESSAGE "Expected '<' to close prolog at line %lu column %lu"
#define ATTRIBUTE_NAME_TOO_LONG_MESSAGE "Attribute name starting with %s is too long."
#define MULTIPLE_NAMESPACES_NOT_SUPPORTED_MESSAGE "Multiple namespaces are not supported at line %lu column %lu."

struct ParseContext {
    FILE* file;
    size_t currentLine;
    size_t currentColumn;
};

struct ParseContext *ParseContext_new(FILE* file, size_t currentLine, size_t currentColumn) {
    struct ParseContext* context = malloc(sizeof(struct ParseContext));

    if (context != NULL) {
        context->file = file;
        context->currentLine = currentLine;
        context->currentColumn = currentColumn;
    }

    return context;
}

bool isLegalElementFirstCharacter(char c);
bool isLegalElementSubsequentCharacter(char c);
bool isLegalAttributeFirstCharacter(char c);
bool isLegalAttributeSubsequentCharacter(char c);
bool isWhitespace(char c);

struct ParseResult parseXmlElement(struct ParseContext *parseContext, struct KeyValueCollection *namespaces);
struct ParseResult parseAttributes(struct ParseContext *parseContext);
struct ParseResult consumeWhitespace(struct ParseContext *parseContext);
struct ParseResult consumeProlog(struct ParseContext *parseContext);

struct ParseResult parseXml(char* path) 
{
    struct ParseResult result = {
        .value = NULL,
        .code = PARSE_XML_SUCCESS
    };

    result.value = Node_new(NODE_TYPE_DOCUMENT);

    if (result.value == NULL) {
        result.code = PARSE_XML_ERROR_MEMORY_ALLOCATION;
        sprintf_s(result.message, sizeof(OUT_OF_MEMORY_MESSAGE), OUT_OF_MEMORY_MESSAGE);

        goto ret;
    }

    FILE* file = fopen(path, "r");
    struct ParseContext* parseContext = ParseContext_new(file, 1, 0);

    consumeWhitespace(parseContext);

    struct KeyValueCollection* namespaces = KeyValueCollection_new();
    char currentChar;

    while ((currentChar = fgetc(file)) != EOF) {
        if (currentChar == '<') {
            parseContext->currentColumn++;

            currentChar = fgetc(file);
			parseContext->currentColumn++;

            if (currentChar == '?') {
                struct ParseResult prologParseResult = consumeProlog(parseContext);

                if (prologParseResult.code != PARSE_XML_SUCCESS) {
                    result.code = prologParseResult.code;

                    goto ret;
				}
            } else {
                ungetc(currentChar, file);
				parseContext->currentColumn--;
            }

            struct ParseResult rootNodeParseResult = parseXmlElement(parseContext, namespaces);

            size_t count = ((struct Node *)result.value)->children->currentEnd;

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
struct ParseResult parseXmlElement(struct ParseContext *parseContext, struct KeyValueCollection *namespaces) 
{
    struct ParseResult result = (struct ParseResult){
        .code = PARSE_XML_SUCCESS,
        .value = NULL
    };

    char currentChar = fgetc(parseContext->file);
    
    if (!isLegalElementFirstCharacter(currentChar)) {
        ungetc(currentChar, parseContext->file);

        result.code = PARSE_XML_INVALID_TAG_CHARACTER;
        snprintf(result.message, sizeof(ILLEGAL_FIRST_CHARACTER_MESSAGE), ILLEGAL_FIRST_CHARACTER_MESSAGE, currentChar, parseContext->currentLine, parseContext->currentColumn);
            
        goto ret;
    }
    else {
		parseContext->currentColumn++;
    }

    char *namespace = DynamicArray_new(128);

    if (namespace == NULL) {
        result.code = PARSE_XML_ERROR_MEMORY_ALLOCATION;
        snprintf(result.message, sizeof(FAILED_TO_ALLOCATE_NAMESPACE_MESSAGE), FAILED_TO_ALLOCATE_NAMESPACE_MESSAGE);

        goto ret;
    }

    char *elementName = DynamicArray_new(128);

    if (elementName == NULL) {
        result.code = PARSE_XML_ERROR_MEMORY_ALLOCATION;
        snprintf(result.message, sizeof(FAILED_TO_ALLOCATE_ELEMENT_NAME_MESSAGE), FAILED_TO_ALLOCATE_ELEMENT_NAME_MESSAGE);

        goto ret;
    }

    bool namespaceFound = false;
    short index = 0;
    char* boxedChar;

    while ((currentChar = fgetc(parseContext->file)) != EOF && !isWhitespace(currentChar)) {
        parseContext->currentColumn++;

        if (!isLegalElementSubsequentCharacter(currentChar)) {
            result.code = PARSE_XML_INVALID_TAG_CHARACTER;
            snprintf(result.message, sizeof(ILLEGAL_ELEMENT_NAME_CHARACTER_MESSAGE), ILLEGAL_ELEMENT_NAME_CHARACTER_MESSAGE, currentChar, parseContext->currentLine, parseContext->currentColumn);
            
            goto ret;
        }

        if (currentChar == ':' && !namespaceFound) {
            memcpy(namespace, elementName, sizeof(namespace));
            memset(elementName, 0, sizeof(elementName));
        }
        else if (currentChar == ':') {
			result.code = PARSE_XML_MULTIPLE_NAMESPACES_NOT_SUPPORTED;
            boxedChar = malloc(1);
            *boxedChar = '\0';
            DynamicArray_add(namespace, boxedChar);
			
			snprintf(result.message, sizeof(MULTIPLE_NAMESPACES_NOT_SUPPORTED_MESSAGE), MULTIPLE_NAMESPACES_NOT_SUPPORTED_MESSAGE, namespace);

            goto ret;
        }
        else {
			boxedChar = malloc(1);
            *boxedChar = currentChar;
            DynamicArray_add(elementName, boxedChar);
        }
    }

    if (currentChar == EOF) {
        result.code = PARSE_XML_UNEXPECTED_EOF;
        snprintf(result.message, sizeof(UNEXPECTED_EOF_MESSAGE), UNEXPECTED_EOF_MESSAGE);
        
        goto ret;
    } 
    
    if (currentChar == '\n') {
        parseContext->currentLine++;
        parseContext->currentColumn = 0;
    }

    result.value = ElementNode_new(namespace, elementName);

	struct ParseResult consumeWhitespaceResult = consumeWhitespace(parseContext);

    if (consumeWhitespaceResult.code != PARSE_XML_SUCCESS) {
        result.code = consumeWhitespaceResult.code;
        snprinf(result.message, sizeof(result.message), consumeWhitespaceResult.message);
    }

    struct ElementNode* elementNode = (struct ElementNode*)result.value;

    struct ParseResult parseAttributesResult = parseAttributes(parseContext);

    if (parseAttributesResult.code != PARSE_XML_SUCCESS) {
        result.code == parseAttributesResult.code;
        snprintf(result.message, sizeof(result.message), parseAttributesResult.message);
		
        goto ret;
    }

    elementNode->attributes = parseAttributesResult.value;

    ret:
        return result;
}

struct ParseResult parseAttributes(struct ParseContext *parseContext)
{
    struct ParseResult result = (struct ParseResult){
        .code = PARSE_XML_SUCCESS,
        .value = NULL
    };

    struct ParseResult consumeWhitespaceResult = consumeWhitespace(parseContext);

    if (consumeWhitespaceResult.code != PARSE_XML_SUCCESS) {
        result = consumeWhitespaceResult;

        goto ret;
    }

    char currentChar;
	short currentValueIndex = 0;
    struct DynamicArray* namespace = DynamicArray_new(16);
	struct DynamicArray* key = DynamicArray_new(64);
	struct DynamicArray* value = DynamicArray_new(64);
    struct KeyValueCollection *attributes = KeyValueCollection_new();
    struct Attribute* attribute;
	struct KeyValue* kvp = KeyValue
    bool namespaceFound = false;
    bool inKey = false;
    bool inValue = false;

    while ((currentChar = fgetc(parseContext->file)) != EOF && currentChar != '>') {
        if (currentChar == '\n') {
            parseContext->currentLine++;
            parseContext->currentColumn = 0;
        }
        else {
            parseContext->currentColumn++;
        }

        if (inKey) {
            if (currentChar == '=' || isWhitespace(currentChar)) {
                inKey = false;
                DynamicArray_add(key, '\0');
                kvp->name = DynamicArray_toString(key);
            } else if (currentChar == ':') {
                if (namespaceFound) {
                    result.code = PARSE_XML_MULTIPLE_NAMESPACES_NOT_SUPPORTED;
                    snprintf(result.message, sizeof(MULTIPLE_NAMESPACES_NOT_SUPPORTED_MESSAGE), MULTIPLE_NAMESPACES_NOT_SUPPORTED_MESSAGE, parseContext->currentLine, parseContext->currentColumn);
                    goto ret;
                } else {
                    namespaceFound = true;
                    attribute->ns = DynamicArray_toString(namespace);
                    DynamicArray_free(key);
					key = DynamicArray_new(64);
				}
            }
            else if (!isLegalElementSubsequentCharacter(currentChar)) {
                result.code = PARSE_XML_INVALID_TAG_CHARACTER;
                snprintf(result.message, sizeof(ILLEGAL_ELEMENT_NAME_CHARACTER_MESSAGE), ILLEGAL_ELEMENT_NAME_CHARACTER_MESSAGE, currentChar, parseContext->currentLine, parseContext->currentColumn);
                
                goto ret;
            }
            else {
				char* boxedChar = malloc(1);
                *boxedChar = currentChar;
                DynamicArray_add(key, boxedChar);
            }
        }
        else if (inValue) {
            
        }
    }

    ret: 
    return result;
}

struct ParseResult consumeWhitespace(struct ParseContext *parseContext) 
{
    struct ParseResult result = {
        .code = PARSE_XML_SUCCESS,
        .value = NULL
	};

    char currentChar;

    while((currentChar = fgetc(parseContext->file)) != EOF && (currentChar == ' ' || currentChar == '\t' || currentChar == '\n')) {
        if (currentChar == '\n') {
            parseContext->currentLine++;
            parseContext->currentColumn = 0;
        } else {
            parseContext->currentColumn++;
        }
	}

    if (currentChar == EOF) {
		result.code = PARSE_XML_UNEXPECTED_EOF;
        snprintf(result.message, sizeof(UNEXPECTED_EOF_MESSAGE), UNEXPECTED_EOF_MESSAGE);
        
        goto ret;
    }

	ungetc(currentChar, parseContext->file);
    parseContext->currentColumn--;
    
    ret:
    return result;
}

/// <summary>
/// Assumes that the <? part of the prolog have already been consumed
/// </summary>
struct ParseResult consumeProlog(struct ParseContext *parseContext) 
{
    struct ParseResult result = {
        .code = PARSE_XML_SUCCESS,
        .value = NULL
	};

    char currentChar = fgetc(parseContext->file);
    parseContext->currentColumn++;

    if (currentChar == EOF) {
		result.code = PARSE_XML_UNEXPECTED_EOF;
		
        goto ret;
    }
	
    if (currentChar != 'x') {
		result.code = PARSE_XML_INVALID_TAG_CHARACTER;
        snprintf(result.message, sizeof(EXPECTED_XML_MESSAGE), EXPECTED_XML_MESSAGE, parseContext->currentLine, parseContext->currentColumn);

        goto ret;
    }

	currentChar = fgetc(parseContext);
	
    if (currentChar != 'm') {
        result.code = PARSE_XML_INVALID_TAG_CHARACTER;
		snprintf(result.message, sizeof(EXPECTED_XML_MESSAGE), EXPECTED_XML_MESSAGE, parseContext->currentLine, parseContext->currentColumn);

		goto ret;
    }

	currentChar = fgetc(parseContext->file);
    parseContext->currentColumn++;

    if (currentChar != 'l') {
        result.code = PARSE_XML_INVALID_TAG_CHARACTER;
		snprintf(result.message, sizeof(EXPECTED_XML_MESSAGE), EXPECTED_XML_MESSAGE, parseContext->currentLine, parseContext->currentColumn);

        goto ret;
    }

	currentChar = fgetc(parseContext->file);
	parseContext->currentColumn++;

    if (currentChar != ' ') {
        result.code = PARSE_XML_INVALID_TAG_CHARACTER;
		snprintf(result.message, sizeof(EXPECTED_WHITESPACE_AFTER_XML_MESSAGE), EXPECTED_WHITESPACE_AFTER_XML_MESSAGE, parseContext->currentLine, parseContext->currentColumn);

        goto ret;
    }

    bool inString = false;
    
    while ((currentChar = fgetc(parseContext->file)) != EOF) {
        if (currentChar = '\n') {
            parseContext->currentLine++;
            parseContext->currentColumn = 0;
        }
        else {
            parseContext->currentColumn++;
        }
        		
        if (currentChar == '"') {
            inString = !inString;
        }
        else if (currentChar == '?' && !inString) {
            currentChar = fgetc(parseContext->file);
            parseContext->currentColumn++;
            if (currentChar != '>') {
                result.code = PARSE_XML_INVALID_TAG_CHARACTER;
                snprintf(result.message, sizeof(EXPECTED_LT_TO_CLOSE_PROLOG_MESSAGE), EXPECTED_LT_TO_CLOSE_PROLOG_MESSAGE, parseContext->currentLine, parseContext->currentColumn);

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
    return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_';
}

bool isLegalElementSubsequentCharacter(char c)
{
    return isLegalElementFirstCharacter(c) || (c >= '0' && c <= '9') || c == '-' || c == '.' || c == ':';
}

bool isLegalAttributeFirstCharacter(char c) 
{
	return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_';
}

bool isLegalAttributeSubsequentCharacter(char c) 
{
	return isLegalAttributeFirstCharacter(c) || (c >= '0' && c <= '9') || c == '-' || c == '.' || c == ':';
}

bool isWhitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n';
}