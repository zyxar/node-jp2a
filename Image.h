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
    if (mNext == INIT || mNext == ALLOC)
      mWidth = w;
  }
  inline int height() const { return mHeight; }
  inline void height(int h) {
    if (mNext == INIT || mNext == ALLOC)
      mHeight = h;
  }
  inline NEXT_STEP next() { return mNext; }

  bool init(FILE *);
  bool init(const char *);
  bool alloc();
  void process();

private:
  bool init();
  void normalize();
  void scanline(const JSAMPLE *);

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
  bool mUsecolors;
  std::stringstream mMessage;
};
} // namespace JP2A
