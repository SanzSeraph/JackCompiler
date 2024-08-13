import Token, { TokenType } from "./token.js";
import ParseError from './parse-error.js';

export default class Tokenizer {
    constructor() {
        
    }

    tokenize(contents) {
        let context = {
            currentTokenString: '',
            inString: false,
            inComment: false,
            currentLine: 0,
            currentColumn: 0,
            tokens: [],
            errors: []
        };

        let i;

        context.peek = () => {
            return contents[i + 1];
        }

        for (i = 0; i < contents.length; i++) {
            context.currentColmn = i;

            let currentChar = contents[i];
            let skipNext = this.parseCurrentChar(context, currentChar);

            if (skipNext) {
                i++;
            }
        }

        if (context.currentTokenString.length > 0) {
            context.tokens.push(new Token)
        }

        if (context.errors.length) {
            for (const error in errors) {
                console.log(`${error.line}, ${error.column}, ${error.message}`);
            }
        } else {
            return context.tokens;
        }
    }

    parseCurrentChar(context, currentChar) {
        if (context.inString) {
            if (currentChar == '"') {
                context.currentTokenString += '"';
                context.inString = false;
                
                context.tokens.push(new Token(context.currentLine, context.currentColumn, context.currentTokenString));
            } else if (currentChar.match(/[\n\r\f]/)) {
                context.errors.push(new ParseError(context.currentLine, context.currentColumn, 'Illegal new line character in string constant.'));
            } else {
                context.curentTokenString += currentChar;
            }
        } else {
            if (currentChar.match(/[\s]/) != null) {
                if (context.currentTokenString.length > 0) {
                    context.tokens.push(new Token(context.currentLine, context.currentColumn, currentTokenString));
                    context.currentTokenString = '';
                }

                if (currentChar == '\n' || currentChar == '\r' && context.peek() == '\f') {
                    context.currentLine++;

                    return true;
                }
            } else if (Token.symbol.includes(currentChar)) {
                if (currentToken.length > 0) {
                    context.tokens.push(new Token(context.currentLine, context.currentColumn, context.currentTokenString));
                }

                context.currentTokenString = currentChar;
                context.tokens.push(new Token(context.currentLine, context.currentColumn, context.currentTokenString));
                context.currentTokenString = '';
            } else if (currentChar == '"') {
                if (context.currentTokenString.length > 0) {
                    if (!Token.match(context.currentTokenString)) {
                        context.errors.push(new ParseError(context.currentLine, context.currentColumn, 'Invalid token'));
                    } else {
                        context.tokens.push(new Token(context.currentLine, context.currentColumn, context.currentTokenString));
                    }
                }
                
                context.currentTokenString = '"';
                context.inString = true;
                inString = false;
            } else {
                context.currentTokenString += currentChar;
            }
        }
    }
}