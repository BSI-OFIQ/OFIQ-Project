#pragma once

#ifdef ANDROID
struct AAsset;
struct AAssetDir;
struct AAssetManager;

namespace OFIQ_LIB
{
  void SetAAssetManager(::AAssetManager* assetManager);

  class asset_streambuf: public std::streambuf
  {
  public:
    asset_streambuf(::AAsset* asset);
    virtual ~asset_streambuf() = default;
  protected:
    virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override;
    virtual pos_type seekpos(pos_type pos, std::ios_base::openmode which) override;
  };
 
  class asset_istream: public std::istream
  {
  public:
    asset_istream(std::string const& filePath, std::ios_base::openmode mode = std::ios_base::in);
    virtual ~asset_istream();
  private:
    ::AAsset* asset;
    asset_streambuf streambuf;
  };

  typedef asset_istream data_source;
}

#else

#include <fstream>

namespace OFIQ_LIB
{
  typedef std::ifstream data_source;
}

#endif

