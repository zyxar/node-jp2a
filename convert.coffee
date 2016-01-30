jp2a = require './build/Release/jp2a'
console.log jp2a.version
console.log jp2a 'test/piccode_10221754_974.jpg'

jp2a = require '.'
console.log jp2a
console.log jp2a.versions
image = new jp2a.Image 'test/piccode_10221754_974.jpg'
console.log image.info()
image.decode (s) -> process.stdout.write(s)
image.decode null
console.log 'closed?', image.close()
