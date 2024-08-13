import fs from 'node:fs/promises';
import path from 'node:path';
import Tokenizer from './tokenizer.js';

let fileOrFolder = process.argv[2];
let stat = await fs.stat(fileOrFolder);

let inputFiles = [];

if (stat.isDirectory()) {
    let dir = await fs.opendir(fileOrFolder);

    for await (const dirEnt of dir) {
        inputFiles.push(path.join(dirEnt.parentPath, dirEnt.name));
    }
} else {
    inputFiles.push(fileOrFolder);
}

let tokenizer = new Tokenizer();

for (const fileName of inputFiles) {
    let inputFile = await fs.open(fileName, 'r');
    let contents = await inputFile.readFile({ encoding: 'utf-8'});
    
    let tokens = tokenizer.tokenize(contents);
    let pathParts = path.parse(fileName);

    let outputFile = path.join(pathParts.base, pathParts.name + '.tokens');

    fs.writeFile(outputFile, tokens, {
        mode: 'c'
    });
}