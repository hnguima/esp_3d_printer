import { getFiles, getDirs, generateIndex, findDuplicates } from "./utils.mjs";
import * as fs from "fs";

import { default as mergedirs } from "merge-dirs";

import { fileURLToPath } from "url";
import * as path from "path";
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

console.log(__dirname);

try {
  fs.rmSync(`./src/app`, { recursive: true });
} catch (error) {}

try {
  fs.rmSync(`./dist`, { recursive: true });
} catch (error) {}

(async () => {
  const files = [];

  for await (const file of getFiles(`${process.env.PWD}/../../../`)) {
    if (
      file.includes("page") &&
      !file.includes("http_server") &&
      !file.includes("build")
    ) {
      files.push(file.split("/").slice(-1)[0]);
      console.log(file);
    }
  }

  if (findDuplicates(files).length > 0) {
    throw "App files with the same name: " + findDuplicates(files).join(", ");
  }

  for await (const dir of getDirs(`${process.env.PWD}/../../../`)) {
    if (
      dir.endsWith("page") &&
      !dir.includes("http_server") &&
      !dir.includes("build")
    ) {
      console.log(dir);
      mergedirs(dir, `${__dirname}/../src/app`);
    }
  }
})().then(() => {
  try {
    generateIndex(`${__dirname}/../src/app/panels`);
    console.log("Successfully collected files");
  } catch (error) {}
});
