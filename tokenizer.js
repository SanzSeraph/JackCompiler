import Token, { TokenType } from "./token.js";

let keyword = [

]

export default class Tokenizer {
    constructor() {
        
    }

    tokenize(contents) {
        this.tokens = [];
        this._contents = contents;

        let currentTokenString = '';
        let inString = false;
        let currentLine = 0;

        for (let i = 0; i < this._contents.length; i++) {
            let currentChar = this._contents[i];
            
            if (inString) {

            } else {           
                if (currentChar.match(/[\s]/) != null) {
                    if (currentToken.length > 0) {
                        this.tokens.push(new Token(currentLine, i, currentTokenString));

                        currentTokenString = '';
                    }
                } else if (Token.symbols.includes(currentChar)) {
                    if (currentToken.length > 0) {
                        this.tokens.push(new Token(currentLine, i, currentTokenString));

                        currentTokenString = currentChar;
                    }
                } else if (currentChar == '"') {
                    currentTokenString += '"';
                    
                    this.tokens.push(new Token(currentLine, i, currentTokenString, TokenType.STRING_CONST));

                    currentTokenString = '';
                    inString = false;
                }
            }
        
        }
    }
}