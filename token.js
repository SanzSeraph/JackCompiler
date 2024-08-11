export default class Token {
    static symbols = [ '(', ')', '{', '}', '[', ']', '.', ',', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'];
    static keywords = [
        'class',
        'constructor',
        'function',
        'method',
        'field',
        'static',
        'var',
        'int',
        'char',
        'boolean',
        'void',
        'true',
        'false',
        'null',
        'this',
        'let',
        'do',
        'if',
        'else',
        'while',
        'return'
    ];

    constructor(line, column, value) {
        this.line = line;
        this.column = column;
        this.value = value;
    }
}

export const TokenType = {
    KEYWORD: 'keyword',
    SYMBOL: 'symbol',
    IDENTIFIER: 'identifier',
    INT_CONST: 'int_const',
    STRING_CONST: 'string_const'
};