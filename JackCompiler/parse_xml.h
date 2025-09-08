#ifndef PARSE_XML_H
#define PARSE_XML_H
#include "node.h"
#include "utf8proc.h"


enum ParseXmlReturnCodes {
    PARSE_XML_SUCCESS = 0,
    PARSE_XML_ERROR_MEMORY_ALLOCATION = 1,
    PARSE_XML_TOO_MANY_ROOT_ELEMENTS = 2,
    PARSE_XML_INVALID_TAG_CHARACTER = 3,
    PARSE_XML_ELEMENT_NAME_TOO_LONG = 4,
    PARSE_XML_UNEXPECTED_EOF = 5
};

struct ParseResult {
    void *value;
    enum ParseXmlReturnCodes code;
    size_t line;
    size_t column;
    char message[128];
};

struct ParseResult parseXml(char* path);

#endif // PARSE_XML_H