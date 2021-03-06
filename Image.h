#ifndef Image_h
#define Image_h

#include <stdio.h>

#include <jpeglib.h>
#include <sstream>
namespace JP2A {

class Image {
public:
  enum NEXT_STEP { INIT, ALLOC, PROCESS, DONE };
  explicit Image();
  ~Image();
  Image &operator>>(std::ostream &);
  inline std::string errorMessage() const { return mMessage.str(); }
  inline const struct jpeg_decompress_struct *jpg() const { return &mJPG; }
  inline int width() const { return mWidth; }
  inline void width(int w) {
    if ((mNext == INIT || mNext == ALLOC) && w > 0)
      mWidth = w;
  }
  inline int height() const { return mHeight; }
  inline void height(int h) {
    if ((mNext == INIT || mNext == ALLOC) && h > 0)
      mHeight = h;
  }
  inline NEXT_STEP next() { return mNext; }
  inline void invert(bool b) { mInvert = b; }
  inline void flipx(bool b) { mFlipX = b; }
  inline void flipy(bool b) { mFlipY = b; }
  inline void color(bool b) {
    if (mNext == INIT || mNext == ALLOC)
      mColor = b;
  }

  bool init(FILE *);
  bool init(const char *);
  bool alloc();
  void process();
  void print_image_colors(FILE *f);

private:
  bool init();
  void normalize();
  void scanline(const JSAMPLE *, int &);
  void aspect_ratio();

private:
  struct jpeg_error_mgr mJerr;
  struct jpeg_decompress_struct mJPG;
  FILE *mFp;
  NEXT_STEP mNext;
  int mWidth;
  int mHeight;
  float *mPixel; // luminosity
  float *mRed, *mGreen, *mBlue;
  int *mYadds;
  float mResizeY;
  float mResizeX;
  int *mLookupResX;
  std::stringstream mMessage;

private:
  float mRedWeight;
  float mGreenWeight;
  float mBlueWeight;
  bool mColor;
  bool mInvert;
  bool mFlipX;
  bool mFlipY;
};
} // namespace JP2A

#endif
