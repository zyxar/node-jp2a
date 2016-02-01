#include "Image.h"
#include <errno.h>
#include <string.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <curses.h>
#include <term.h>
#endif

namespace JP2A {
#define ROUND(x) (int)(0.5f + x)
#define ASCII_PALETTE_SIZE 256
char ascii_palette[ASCII_PALETTE_SIZE + 1] = "   ...',;:clodxkO0KXNWM";

bool termsize(int *_width, int *_height) {
#ifdef WIN32
  CONSOLE_SCREEN_BUFFER_INFO io;
  if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &io)) {
    return false;
  }
  *_width = io.srWindow.Right - io.srWindow.Left;
  *_height = io.srWindow.Bottom - io.srWindow.Top;
  return true;
#else
  char *termtype = getenv("TERM");
  char term_buffer[2048];
  if (!termtype) {
    return false;
  }

  int i = tgetent(term_buffer, termtype);
  if (i <= 0) {
    return false;
  }
  *_width = tgetnum((char *)"co");
  *_height = tgetnum((char *)"li");
  return true;
#endif
}

Image::Image()
    : mJerr{}, mJPG{}, mFp{nullptr}, mNext{INIT}, mWidth{0}, mHeight{0},
      mPixel{nullptr}, mRed{nullptr}, mGreen{nullptr}, mBlue{nullptr},
      mYadds{nullptr}, mLookupResX{nullptr}, mMessage{}, mRedWeight{0.2989f},
      mGreenWeight{0.5866f}, mBlueWeight{0.1145f}, mUseBorder{0},
      mUsecolors{false}, mInvert{true}, mFlipX{false}, mFlipY{false} {
  for (int n = 0; n < 256; ++n) {
    mRED[n] = ((float)n) * mRedWeight / 255.0f;
    mGREEN[n] = ((float)n) * mGreenWeight / 255.0f;
    mBLUE[n] = ((float)n) * mBlueWeight / 255.0f;
    mGRAY[n] = ((float)n) / 255.0f;
  }
}

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
  aspect_ratio();
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
          v += mGRAY[src[0]];
        else {
          v += mRED[src[0]] + mGREEN[src[1]] + mBLUE[src[2]];
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

void Image::aspect_ratio() {
  // the 2.0f and 0.5f factors are used for text displays that (usually) have
  // characters that are taller than they are wide.
  int tWidth = 0, tHeight = 0;
  if (!termsize(&tWidth, &tHeight)) {
    return; // TODO: throw exception
  }

  if (mWidth == 0) {
    if (mHeight == 0) {
      mWidth = tWidth;
      mHeight = ROUND(0.5f * (float)mWidth * (float)mJPG.output_height /
                      (float)mJPG.output_width);
    } else {
      mWidth = ROUND(2.0f * (float)mHeight * (float)mJPG.output_width /
                     (float)mJPG.output_height);
      while (mWidth == 0) {
        ++mHeight;
        mWidth = ROUND(2.0f * (float)mHeight * (float)mJPG.output_width /
                       (float)mJPG.output_height);
      }
    }
  } else {
    if (mHeight == 0) {
      mHeight = ROUND(0.5f * (float)mWidth * (float)mJPG.output_height /
                      (float)mJPG.output_width);
      while (mHeight == 0) {
        ++mWidth;
        mHeight = ROUND(0.5f * (float)mWidth * (float)mJPG.output_height /
                        (float)mJPG.output_width);
      }
    }
  }

  if ((mWidth + mUseBorder * 2) > tWidth) {
    mWidth = tWidth - mUseBorder * 2;
    mHeight = ROUND(0.5f * (float)mWidth * (float)mJPG.output_height /
                    (float)mJPG.output_width);
  }
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
      const float lum = mPixel[x + (mFlipY ? mHeight - y - 1 : y) * mWidth];
      const int pos = ROUND((float)chars * lum);
      line[mFlipX ? mWidth - x - 1 : x] =
          ascii_palette[mInvert ? pos : chars - pos];
    }
    os << line << "\n";
  }
#ifdef WIN32
  free(line);
#endif
  return *this;
}
}
