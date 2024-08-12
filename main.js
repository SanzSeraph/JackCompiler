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

for (fileName of inputFiles) {
    let inputFile = await fs.open(fileName, 'r');
    let contents = await inputFile.read()
    
    let tokens = tokenizer.tokenize(contents);

}