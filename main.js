import fs from 'node:fs/promises';
import path from 'node:path';

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

