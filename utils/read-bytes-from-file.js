'use strict';

let fs = require('fs');

let buffer = fs.readFileSync(process.argv[2]);

let array = [];
for (let i = 0; i < buffer.length; ++i) {
  array.push(buffer.readUInt8(i));
}

console.log(array.join(' '));
