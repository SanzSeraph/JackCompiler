#ifndef PARSE_XML_H
#define PARSE_XML_H
#include "node.h"
#include "utf8proc.h"


enum ParseXmlReturnCodes {
    PARSE_XML_SUCCESS,
    PARSE_XML_ERROR_MEMORY_ALLOCATION,
    PARSE_XML_TOO_MANY_ROOT_ELEMENTS,
    PARSE_XML_INVALID_TAG_CHARACTER,
	PARSE_XML_MULTIPLE_NAMESPACES_NOT_SUPPORTED,
    PARSE_XML_ELEMENT_NAME_TOO_LONG,
	PARSE_XML_ATTRIBUTE_NAME_TOO_LONG,
	PARSE_XML_ATTRIBUTE_VALUE_TOO_LONG,
    PARSE_XML_UNEXPECTED_EOF
};

struct ParseResult {
    void *value;
    enum ParseXmlReturnCodes code;
    char message[128];
};

struct ParseResult parseXml(char* path);

#endif // PARSE_XML_H