export default class ParseError {
    constructor(fileName, lineNumber, columnNumber, message) {
        this.fileName = fileName;
        this.lineNumber = lineNumber;
        this.columnNumber = columnNumber;
        this.message = message;
    }
}