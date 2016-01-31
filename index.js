/*global require,module*/
(function () {
  'use strict';
  const jp2a = require('./build/Release/jp2a');
  Object.defineProperty(jp2a, 'versions', {
    writeable: false,
    value: {
      module: require('./package').version,
      jp2a: jp2a.version
    }
  });
  module.exports = jp2a;
})();
