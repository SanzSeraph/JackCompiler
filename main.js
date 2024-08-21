import fs from 'node:fs/promises';
import path from 'node:path';
import Tokenizer from './tokenizer.js';
import jsonFormat from 'json-format';

let fileOrFolder = process.argv[2];
let stat = await fs.stat(fileOrFolder);

let inputFiles = [];

if (stat.isDirectory()) {
    let dir = await fs.opendir(fileOrFolder);

    for await (const dirEnt of dir) {
        let parts = path.parse(dirEnt.name);

        if (parts.ext == '.jack') {
            inputFiles.push(path.join(dirEnt.parentPath, dirEnt.name));
        }
    }
} else {
    inputFiles.push(fileOrFolder);
}


for (const fileName of inputFiles) {
    let inputFile = await fs.open(fileName, 'r');
    let contents = await inputFile.readFile({ encoding: 'utf-8'});
    
    let tokenizer = new Tokenizer(fileName, contents);

    if (tokenizer.errors.length) {
        for (const error of tokenizer.errors) {
            console.log(`${error.fileName}, ${error.lineNumber}, ${error.columnNumber} ${error.message}`);
        }
    } else {
        let pathParts = path.parse(fileName);

        let outputFile = path.join(pathParts.dir, pathParts.name + '.tokens');
    
        await fs.writeFile(outputFile, jsonFormat(tokenizer.tokens));
    
    }
    
    inputFile.close();
}