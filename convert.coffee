jp2a = require './build/Release/jp2a'
console.log jp2a.version
jp2a 'test/piccode_10221754_974.jpg'

jp2a = require '.'
console.log jp2a.versions
jp2a 'test/piccode_10221754_974.jpg'
