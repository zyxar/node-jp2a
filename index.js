/*global require,module*/
(function () {
  'use strict';
  var jp2a = require('./build/Release/jp2a');
  var pack = require('./package');
  Object.defineProperty(jp2a, 'versions', {
    writeable: false,
    value: {
      module: pack.version,
      jp2a: jp2a.version
    }
  });
  module.exports = jp2a;
})();
