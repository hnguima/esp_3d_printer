import * as fs from "fs";
import { readdir } from "fs/promises";

import { resolve } from "path";
import { relative } from "path";

export async function* getFiles(dir) {
  const dirents = await readdir(dir, { withFileTypes: true });
  for (const dirent of dirents) {
    const res = resolve(dir, dirent.name);
    if (dirent.isDirectory()) {
      yield* getFiles(res);
    } else {
      yield res;
    }
  }
}

export async function* getDirs(dir) {
  const dirents = await readdir(dir, { withFileTypes: true });
  for (const dirent of dirents) {
    const res = resolve(dir, dirent.name);
    if (dirent.isDirectory()) {
      yield* getDirs(res);
    }
  }
  yield dir;
}

export async function generateIndex(dir) {
  for await (const file of getFiles(dir)) {
    var text = fs.readFileSync(file, "utf8");

    while (text.length != 0) {

      const match = text.match(/(?:export(?: \w+ | { *))(\w*)/);

      if (!match) {
        break;
      }

      text = text.slice(match.index).split(/\n(.*)/s)[1];

      console.log(relative(resolve(dir), file));

      fs.appendFile(
        `./src/app/panels/index.js`,
        `export { ${match[1]} } from "./${relative(resolve(dir), file)}";\n`,
        function (err) {
          if (err) throw err;
        }
      );
    }
  }
}

export const findDuplicates = (arr) =>
  arr.filter((item, index) => arr.indexOf(item) != index);
