#!/usr/bin/env node
const jp2a = require('../');
var args = process.argv;
if (args.length <= 2) {
  var package = require('../package.json');
  process.stdout.write(package.name+' '+package.version+'\n'+package.description+'\n');
  process.exit(0);
}

var option = {
  color: true,
  invert: false,
  flipx: false,
  flipy: false};

args.slice(2).map(function (path) {
  process.stdout.write(jp2a(path, option));
});
