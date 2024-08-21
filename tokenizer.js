import Token, { TokenType } from "./token.js";
import ParseError from './parse-error.js';

export default class Tokenizer {
    constructor(fileName, contents) {
        this.fileName = fileName;
        this.contents = contents;
        this.currentTokenString = '';
        this.inString = false;
        this.inComment = false;
        this.inSingleLineComment = false;
        this.currentLine = 0;
        this.currentColumn = 0;
        this.tokens = [];
        this.errors = [];
        this.currentIndex = 0;

        this.tokenize();
    }

    peek() {
        return this.contents[this.currentIndex + 1];
    }

    tokenize() {
        for (this.currentIndex; this.currentIndex < this.contents.length; this.currentIndex++) {
            this.currentColumn += 1;

            let currentChar = this.contents[this.currentIndex];
            let skipNext = this.parseCurrentChar(currentChar);

            if (skipNext) {
                this.currentIndex++;
            }
        }
    }

    parseCurrentChar(currentChar) {
        let skipNext = false;

        if (this.inString) {
            if (currentChar == '"') {
                this.currentTokenString += '"';
                this.inString = false;
                
                this.tokens.push(new Token(this.currentLine, this.currentColumn, this.currentTokenString, TokenType.STRING_CONST));
            } else if (currentChar.match(/[\n\r]/)) {
                this.errors.push(new ParseError(this.fileName, this.currentLine, this.currentColumn, 'Illegal new line character in string constant.'));
            } else {
                this.curentTokenString += currentChar;
            }
        } else if (this.inComment) {
            if (currentChar == '*' && this.peek() == '/') {
                this.inComment = false;

                skipNext = true;
            }
        } else if (this.inSingleLineComment) {
            if (currentChar == '\n') {
                this.inSingleLineComment = false;
            } else if (currentChar == '\r' && this.peek() == '\n') {
                this.inSingleLineComment = false;

                skipNext = true;
            }
        } else {
            if (currentChar == '/' && this.peek() == '/') {
                this.inSingleLineComment = true;

                skipNext = true;
            } else if (currentChar == '/' && this.peek() == '*') {
                this.inComment = true;

                skipNext = true;
            } else if (currentChar.match(/[\s]/) != null) {
                if (this.currentTokenString.length > 0) {
                    this.tokens.push(new Token(this.currentLine, this.currentColumn, this.currentTokenString));
                    this.currentTokenString = '';
                }
            } else if (Token.symbol.includes(currentChar)) {
                if (this.currentTokenString.length > 0) {
                    this.tokens.push(new Token(this.currentLine, this.currentColumn, this.currentTokenString));
                }

                this.currentTokenString = currentChar;
                this.tokens.push(new Token(this.currentLine, this.currentColumn, this.currentTokenString));
                this.currentTokenString = '';
            } else if (currentChar == '"') {
                if (this.currentTokenString.length > 0) {
                    if (!Token.match(this.currentTokenString)) {
                        this.errors.push(new ParseError(this.fileName, this.currentLine, this.currentColumn, 'Invalid token'));
                    } else {
                        this.tokens.push(new Token(this.currentLine, this.currentColumn, this.currentTokenString));
                    }
                }
                
                this.currentTokenString = '"';
                this.inString = true;
            }
            else {
                this.currentTokenString += currentChar;
            }
        }

        if (currentChar == '\n' || currentChar == '\r' && this.peek() == '\n') {
            this.currentLine++;
            this.currentColumn = 0;

            skipNext = true;
        }

        return skipNext;
    }
}