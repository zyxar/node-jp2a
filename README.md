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

This Node.js module is derived from [jp2a](https://github.com/cslarsen/jp2a).

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

                 ..
        .'cdo;;ok0O,
       ;ONNd.  .:kXl  ':dl;,;odc'.                  .,,,.
      '0MMNx;.    :lckNXl.   .dNNOc;;,,'            .:OWXkc;. .;;,,,'.
      .OWMMMWKkxxdcdNWXc      .oWMMMWMMKl,;oxdc'     .dWWO'   .lOOdlc,...........''''''''''''..
      .oKWMMWWMMMMMMMW0'       ;KMMMMMWd..'oXWMNx:,;;lKWWO:;odool;,,;;;,,,,,,,,,,,,,,,,,,,;;,'.
      .',,;ldxOKNMMMWW0:......':0WMMMMNx;,;:kWMM0,   :XWWklx0o.
      .dx'     .cKMMWWXl,;;;,,,c0WWMMWX;  .cKMWKc.   lNWKoxNWK:
       lWXxc,;;;dXKXWWXc       ;KWWMWWKook0NXd;.    .kWWk..oNWNl.
       'oc,:okd:,,.,OWM0,     .xWWNWWWk.;0WWXc   .,,oXMWd. .cXMNd.
                    .;d0Ko:;;l00d:xWMWo  'OWMXo. .,,;okxl;.  :XMWO:.
                       .';lKMMWx;:OWWWd   .kWMWk'             .,,,,'.
                           :KWMWN0kKKko:.  .xWWWKo;,.
                            .,lxo:,,.       .',;;;,,.


Besides, `jp2a.Image` is exported to wrap underlying image and works the async way:

```coffeescript
jp2a = require 'node-jp2a'
image = new jp2a.Image 'test/piccode_10221754_974.jpg'
image.decode {
  width: 120,
  color: true,
  invert: true,
  flipx: false,
  flipy: false}, (err, text) ->
  if err then process.stderr.write err+'\n'
  else process.stdout.write text
  image.close()
```

    MMMMMMMMMMMMMMMWWWMMMWWMMMMWMMMWMMMMMMMMMWWWMMMMMMMMMMMMMMMMMMMWWMMWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMMMMMMXOkxxkkxxooKMMWWMMMMMMMMMMMMMWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMWMMMMMWXd:'',cxxc'. .dWMMMNOxkkkkxO0NMMMWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMWMMMMN0l  .xWMWWMXOl.cWMXx;..cclc. .;dKWMWWMMMMMMMMMMMMMWWMWOllllxXWMWMMWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMWWOc.   ;xxKWMMWNdloc, .lKWWMW0:.  'oxxxkkO0NMMMMMMMMWWMWKd'  .,oxx0WXxxkkkkOOXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMWM0o'      .,;;,:o:   .oNWWMMMMX0c        .;lkxc;;:oONWWMWX:   'ONNNNKl'.':lookKKKKKKKKKKKK0OOOOOOOOOOOOOOO0XWMMMMM
    MMMMMMW0d;                 .OMWWMMMMWWd.       ;OXMWx.   .:dddl:.   .clllc'.'ccllllllllllllllllllllllllllllllllldKWMMMMM
    MMMMMMWX0kl;'..            '0MMMMMMMWWO'       'odkkd'    .lkxx:    ,oc;,;dkOO0KKKKKKKKKKKKKKKKKXNNNNNNNNNNNNNNNWWMMMMMM
    MMMMMWWXOdkNWX0Okdl,.      .lkxxxxkkkxo.       :kOKX0;    ,0WWWo    ;;   :0WMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMWX0;.dNWMMWWWOl'     .d00OO00XXXk'      .xWMMK:    .kWWWNc   'kl    cXMMMMMWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMNNl .;okkxxx:....   .oWMWMMMMWWx.      .coc,. ..:x0WWWWK,   ,0Xc.   lXMMMMWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMMMOcokdoc,:dkxkKk'   .kWMMMWWKx;       ,0Xx.  ..oNWWKkkc.   :XMX0o.  :xKMWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMMMMMMMMMMWWWWMMWMKxc. .xXNWWO,  .:;    :XMWd.   .dWWOllc.   .cOWWNc   .;KWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMMMMMWMMMMMMWWMMMMMMNOo:col:,.  ;OK:    cNWMNo.   .dNWWWW0xxxxxKMWW0c;;;:dOXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWWK;    .:l:'.  'dk0WNd.   .:0WWMMWWWMMMMMMMMMMMMMXXMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMO'    .;c:..clllkWWNl.    'co0MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWKkl;;cdkkkKWWWWWWWWX0OkxxxkkXWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWWMMWMMMMWWWWWWWWWWMWWWWMMWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMWMMMMMMWMMMMMMMMMWWWWMMMMMMWWWWMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM


## License
[GPL-2.0](http://opensource.org/licenses/GPL-2.0)
