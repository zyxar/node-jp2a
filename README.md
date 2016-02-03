node-jp2a
=========
[![NPM](https://nodei.co/npm/node-jp2a.png?downloads=true)](https://nodei.co/npm/node-jp2a/)

[![Build Status](https://api.travis-ci.org/zyxar/node-jp2a.svg?branch=master)](https://travis-ci.org/zyxar/node-jp2a)

jp2a library for Node.js


## Prerequisites

- Node.js 4.x/5.x
- C++11 compiler
- `npm install -g node-gyp`
- [libjpeg](http://www.ijg.org)

  - OS X: `brew install jpeg`
  - Ubuntu: `sudo aptitude install libjpeg-dev`

## Install

- From NPM: `npm install node-jp2a --save`

## Brief introduction

This Node.js module is derived from [jp2a](http://sourceforge.net/projects/jp2a/).

This module exports a `jp2a` function as a convienent function to convert a JPEG image to ascii text, which accpets a string (path of the image) and an optional JSON object as parameters. `jp2a` function throws exception on encountering errors.

```coffeescript
jp2a = require 'node-jp2a'
console.log jp2a 'test/piccode_10221754_974.jpg', {
  width: 100,
  height: 18,
  color: true,
  invert: false,
  flipx: false,
  flipy: false}
```

Besides, `jp2a.Image` is exported to wrap underlying image and works the async way:

```coffeescript
jp2a = require 'node-jp2a'
image = new jp2a.Image 'test/piccode_10221754_974.jpg'
image.decode {
  width: 100,
  height: 18,
  color: true,
  invert: false,
  flipx: false,
  flipy: false}, (err, text) ->
  if err then process.stderr.write err+'\n'
  else process.stdout.write text
  image.close()
```

## License
[GPL-2.0](http://opensource.org/licenses/GPL-2.0)
