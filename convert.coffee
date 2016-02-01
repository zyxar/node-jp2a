jp2a = require './build/Release/jp2a'
console.log jp2a.version
console.log jp2a 'test/piccode_10221754_974.jpg'

jp2a = require '.'
console.log jp2a
console.log jp2a.versions
image = new jp2a.Image 'test/piccode_10221754_974.jpg'
image.decode null
image.decode (e, s) ->
  if e then process.stderr.write e+'\n'
  else
    process.stdout.write s
    console.log image.info()
console.log 'closed?', image.close()
image.decode (e, s) ->
  if e then process.stderr.write e+'\n'
  else process.stdout.write s

image = new jp2a.Image 'test/piccode_10221754_974.jpg'
image.decode 100, 18, (e, s) ->
  if e then process.stderr.write e+'\n'
  else process.stdout.write s
  image.close()
