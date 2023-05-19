import { getFiles } from "./utils.mjs";
import * as fs from "fs";

import { fileURLToPath } from "url";
import * as path from "path";
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

var buildDir = process.argv[2] || process.env.BUILD_DIR || "./../../build";
try {
  fs.rmSync(`${buildDir}/server/`, {recursive: true});
} catch (error) {}
fs.mkdirSync(`${buildDir}/server/`, { recursive: true });

(async () => {
  const files = [];

  for await (const file of getFiles(`${__dirname}/../dist/srv`)) {
    console.log(
      path.relative(`${__dirname}/../dist/srv`, file).split(".gz")[0]
    );
    fs.copyFileSync(
      file,

      `${buildDir}/server/${
        path.relative(`${__dirname}/../dist/srv`, file).split(".gz")[0]
      }`
    );
  }
})();

console.log(buildDir);
