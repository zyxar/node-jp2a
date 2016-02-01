#include "Image.h"
#include "jp2a-1.0.6/include/options.h"
#include <errno.h>
#include <string.h>

namespace JP2A {
#define ROUND(x) (int)(0.5f + x)

extern "C" {
extern void aspect_ratio(const int, const int);
}

Image::Image()
    : mJerr{}, mJPG{}, mFp{nullptr}, mNext{INIT}, mWidth{0}, mHeight{0},
      mPixel{nullptr}, mRed{nullptr}, mGreen{nullptr}, mBlue{nullptr},
      mYadds{nullptr}, mLookupResX{nullptr}, mUsecolors{false}, mMessage{} {}

bool Image::init() {
  mJPG.err = jpeg_std_error(&mJerr);
  jpeg_create_decompress(&mJPG);
  jpeg_stdio_src(&mJPG, mFp);
  jpeg_read_header(&mJPG, TRUE);
  jpeg_start_decompress(&mJPG);
  if (mJPG.data_precision != 8) {
    mMessage << "Image has " << mJPG.data_precision
             << " bits color channels, we only support 8-bit.";
    return false;
  }
  aspect_ratio(mJPG.output_width, mJPG.output_height);
  mWidth = ::width;
  mHeight = ::height;
  mNext = ALLOC;
  return true;
}

bool Image::init(const char *filename) {
  mFp = fopen(filename, "r");
  if (!mFp) {
    mMessage << strerror(errno);
    return false;
  }
  return this->init();
}

bool Image::init(FILE *fp) {
  if (fp == nullptr) {
    mMessage << "Null file pointer.";
    return false;
  }
  mFp = fp;
  return this->init();
}

bool Image::alloc() {
  mYadds = (int *)malloc(mHeight * sizeof(int));
  mPixel = (float *)malloc(mWidth * mHeight * sizeof(float));
  mLookupResX = (int *)malloc((1 + mWidth) * sizeof(int));
  if (mUsecolors) {
    mRed = (float *)malloc(mWidth * mHeight * sizeof(float));
    mGreen = (float *)malloc(mWidth * mHeight * sizeof(float));
    mBlue = (float *)malloc(mWidth * mHeight * sizeof(float));
  }
  if (!(mPixel && mYadds && mLookupResX) ||
      (mUsecolors && !(mRed && mGreen && mBlue))) {
    mMessage << "Not enough memory for given output dimension.";
    return false;
  }
  memset(mYadds, 0, mHeight * sizeof(int));
  memset(mPixel, 0, mWidth * mHeight * sizeof(float));
  memset(mLookupResX, 0, (1 + mWidth) * sizeof(int));
  if (mUsecolors) {
    memset(mRed, 0, mWidth * mHeight * sizeof(float));
    memset(mGreen, 0, mWidth * mHeight * sizeof(float));
    memset(mBlue, 0, mWidth * mHeight * sizeof(float));
  }
  mResizeY = (float)(mHeight - 1) / (float)(mJPG.output_height - 1);
  mResizeX = (float)(mJPG.output_width - 1) / (float)(mWidth);
  for (int j = 0; j <= mWidth; ++j) {
    mLookupResX[j] = ROUND((float)j * mResizeX);
    mLookupResX[j] *= mJPG.out_color_components;
  }
  mNext = PROCESS;
  return true;
}

void Image::process() {
  int row_stride = mJPG.output_width * mJPG.output_components;
  JSAMPARRAY jbuffer = (*mJPG.mem->alloc_sarray)(
      reinterpret_cast<j_common_ptr>(&mJPG), JPOOL_IMAGE, row_stride, 1);
  while (mJPG.output_scanline < mJPG.output_height) {
    jpeg_read_scanlines(const_cast<j_decompress_ptr>(&mJPG), jbuffer, 1);
    scanline(jbuffer[0]);
  }
  normalize();
  mNext = DONE;
}

#define TRY_(func, ptr)                                                        \
  {                                                                            \
    if (ptr) {                                                                 \
      func(ptr);                                                               \
      ptr = nullptr;                                                           \
    }                                                                          \
  }

Image::~Image() {
  jpeg_finish_decompress(&mJPG);
  jpeg_destroy_decompress(&mJPG);
  TRY_(fclose, mFp);
  TRY_(free, mPixel);
  TRY_(free, mRed);
  TRY_(free, mGreen);
  TRY_(free, mBlue);
  TRY_(free, mYadds);
  TRY_(free, mLookupResX);
}

void Image::scanline(const JSAMPLE *sampleline) {
  static int lasty = 0;
  const int y = ROUND(mResizeY * (float)(mJPG.output_scanline - 1));

  // include all scanlines since last call
  float *pixel, *red, *green, *blue;
  pixel = &mPixel[lasty * mWidth];
  red = green = blue = nullptr;

  if (mUsecolors) {
    int offset = lasty * mWidth;
    red = &mRed[offset];
    green = &mGreen[offset];
    blue = &mBlue[offset];
  }

  while (lasty <= y) {
    const int components = mJPG.out_color_components;
    const int readcolors = mUsecolors;
    for (int x = 0; x < mWidth; ++x) {
      const JSAMPLE *src = &sampleline[mLookupResX[x]];
      const JSAMPLE *src_end = &sampleline[mLookupResX[x + 1]];
      int adds = 0;
      float v, r, g, b;
      v = r = g = b = 0.0f;
      while (src <= src_end) {
        if (components != 3)
          v += GRAY[src[0]];
        else {
          v += RED[src[0]] + GREEN[src[1]] + BLUE[src[2]];
          if (readcolors) {
            r += (float)src[0] / 255.0f;
            g += (float)src[1] / 255.0f;
            b += (float)src[2] / 255.0f;
          }
        }
        ++adds;
        src += components;
      }
      pixel[x] += adds > 1 ? v / (float)adds : v;
      if (readcolors) {
        red[x] += adds > 1 ? r / (float)adds : r;
        green[x] += adds > 1 ? g / (float)adds : g;
        blue[x] += adds > 1 ? b / (float)adds : b;
      }
    }

    ++mYadds[lasty++];
    pixel += mWidth;
    if (readcolors) {
      red += mWidth;
      green += mWidth;
      blue += mWidth;
    }
  }

  lasty = y;
}

void Image::normalize() {
  float *pixel = mPixel;
  float *red = mRed;
  float *green = mGreen;
  float *blue = mBlue;
  for (int y = 0; y < mHeight; ++y) {
    if (mYadds[y] > 1) {
      for (int x = 0; x < mWidth; ++x) {
        pixel[x] /= mYadds[y];
        if (mUsecolors) {
          red[x] /= mYadds[y];
          green[x] /= mYadds[y];
          blue[x] /= mYadds[y];
        }
      }
    }
    pixel += mWidth;
    if (mUsecolors) {
      red += mWidth;
      green += mWidth;
      blue += mWidth;
    }
  }
}

Image &Image::operator>>(std::ostream &os) {
#ifdef WIN32
  char *line = (char *)malloc(mWidth + 1);
#else
  char line[mWidth + 1];
#endif
  int chars = (int)strlen(ascii_palette) - 1;
  line[mWidth] = 0;
  for (int y = 0; y < mHeight; ++y) {
    for (int x = 0; x < mWidth; ++x) {
      const float lum = mPixel[x + (flipy ? mHeight - y - 1 : y) * mWidth];
      const int pos = ROUND((float)chars * lum);
      line[flipx ? mWidth - x - 1 : x] =
          ascii_palette[invert ? pos : chars - pos];
    }
    os << line << "\n";
  }
#ifdef WIN32
  free(line);
#endif
  return *this;
}
}
