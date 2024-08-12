export default class ParseError {
    constructor(lineNumber, columnNumber, message) {
        this.lineNumber = lineNumber;
        this.columnNumber = columnNumber;
        this.message = message;
    }
}