export default class Token {
    static symbol = [ '(', ')', '{', '}', '[', ']', '.', ',', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'];
    static keyword = [
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
    static identifier = /[A-Za-z_]+[A-Za-z1-9_]*/;
    static intconst = /[\d]*/;
    static intmax = 32767;
    static intmin = 0;

    static match(str) {
        if (Token.symbol.includes(str)) {
            return true;
        }

        if (Token.keyword.includes(str)) {
            return true;
        }

        if (str.match(Token.identifier) != null) {
            return true;
        }

        if (str.match(Token.intconst) != null) {
            let val = parseInt(str);

            if (val > Token.intMax) {
                return false;
            }

            if (val < Token.intmin) {
                return false;
            }
        }

        return false;
    }

    constructor(line, column, value, type) {
        this.line = line;
        this.column = column;
        this.value = value;
        
        if (type) {
            this.type = type;
        } else if (Token.keyword.includes(value)) {
            this.type = TokenType.KEYWORD;
        } else if (Token.symbol.includes(value)) {
            this.type = TokenType.SYMBOL;
        } else if (value.match(Token.intconst)) {
            this.type = TokenType.INT_CONST;
        } else if (value.match(Token.identifier)) {
            this.type = TokenType.IDENTIFIER;
        }
    }
}

export const TokenType = {
    KEYWORD: 'keyword',
    SYMBOL: 'symbol',
    IDENTIFIER: 'identifier',
    INT_CONST: 'int_const',
    STRING_CONST: 'string_const'
};