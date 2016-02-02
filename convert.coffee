jp2a = require '.'
console.log jp2a 'test/piccode_10221754_974.jpg', {
  invert: false,
  flipx: true,
  flipy: false}

image = new jp2a.Image 'test/piccode_10221754_974.jpg'
image.decode null
image.decode {color:true}, (e, s) ->
  if e then process.stderr.write e+'\n'
  else
    process.stdout.write s
    console.log jp2a
    console.log image.info()
console.log 'closed?', image.close()
image.decode (e, s) ->
  if e then process.stderr.write e+'\n'
  else process.stdout.write s

image = new jp2a.Image 'test/piccode_10221754_974.jpg'
image.decode {
  width: 100,
  height: 18,
  color: true,
  invert: false,
  flipx: false,
  flipy: true}, (e, s) ->
  if e then process.stderr.write e+'\n'
  else process.stdout.write s
  image.close()
