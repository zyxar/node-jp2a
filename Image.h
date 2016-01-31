#include <stdio.h>
#include <jpeglib.h>
#include <sstream>
namespace JP2A {

class Image {
public:
  explicit Image(unsigned char *, int);
  ~Image();
  Image &operator>>(std::ostream &);
  inline std::string errorMessage() const { return mMessage.str(); }
  inline const struct jpeg_decompress_struct *jpg() const { return &mJPG; }
  inline int width() const { return mWidth; }
  inline void width(int w) { mWidth = w; }
  inline int height() const { return mHeight; }
  inline void height(int h) { mHeight = h; }

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
