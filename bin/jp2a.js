#!/usr/bin/env node
(function () {
  const jp2a = require('../');
  var args = process.argv;
  if (args.length <= 2) {
    var package = require('../package.json');
    process.stdout.write(package.name+' '+package.version+'\n');
    process.exit(0);
  }

  const option = {
    color: false,
    invert: false,
    flipx: false,
    flipy: false};

  const optionKeys = {
    width: 'number',
    height: 'number',
    color: 'boolean',
    invert: 'boolean',
    flipx: 'boolean',
    flipy: 'boolean'};

  args.slice(2).map(function (arg) {
    if (arg.indexOf('--') === 0) {
      arg = arg.substr(2).split('=');
      if (arg.length >= 1) {
        var key = arg[0];
        if (optionKeys.hasOwnProperty(key)) {
          if (arg.length === 1) {
            if (optionKeys[key] === 'boolean') {
              option[key] = true;
            }
          } else {
            try {
              var value = JSON.parse(arg[1]);
              if (typeof value === optionKeys[key]) {
                option[key] = value;
              }
            } catch (e) {}
          }
        }
      }
    } else {
      return arg;
    }
  }).filter(function (v) {
    return v !== undefined;
  }).map(function (path) {
    process.stdout.write(jp2a(path, option));
  });
}());
