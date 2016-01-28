#include <stdio.h>
#include <jpeglib.h>
#include <sstream>
namespace JP2A {

class Image {
public:
  explicit Image(unsigned char *, int);
  ~Image();
  Image &operator>>(std::ostream &);
  inline std::string errorMessage() { return mMessage.str(); }

private:
  void normalize();
  void scanline(const JSAMPLE *);

private:
  struct jpeg_error_mgr mJerr;
  struct jpeg_decompress_struct mJPG;
  int mWidth;
  int mHeight;
  float *mPixel; // luminosity
  float *mRed, *mGreen, *mBlue;
  int *mYadds;
  float mResizeY;
  float mResizeX;
  int *mLookupResX;
  bool mReady;
  bool mUsecolors;
  std::stringstream mMessage;
};
} // namespace JP2A
