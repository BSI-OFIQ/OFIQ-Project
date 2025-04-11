#include "data_source.h"
#include <istream>

#ifdef ANDROID
#include <android/asset_manager.h>
#include <android/log.h>
#include <string>

static AAssetManager* g_assetManager;

namespace OFIQ_LIB
{
  using pos_type=std::streambuf::pos_type;
  using off_type=std::streambuf::off_type;

  void SetAAssetManager(::AAssetManager* assetManager)
  {
    g_assetManager = assetManager;
  }

  asset_streambuf::asset_streambuf(::AAsset* asset)
  {
    char* beg(const_cast<char*>(static_cast<char const*>(::AAsset_getBuffer(asset))));
    char* end(beg + ::AAsset_getLength(asset));
    __android_log_print(ANDROID_LOG_INFO, __FUNCTION__, "streambuf for asset %p: %p .. %p", asset, beg, end);
       
    setg(beg, beg, end);
  }

  pos_type asset_streambuf::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which)
  {
    pos_type result(off_type(-1));
    if ((which & (std::ios_base::in | std::ios_base::out)) == std::ios_base::in)
    {
      switch(dir)
      {
        case std::ios::beg:
          result = off;
	  break;
        case std::ios::end:
	  result = (egptr() - eback()) - (off + 1);
          break;
    	case std::ios::cur:
	  result = (gptr() - eback()) + off;
	  break;
        default:
	  return result;
      }
      return(seekpos(result, which)); 
    }
    return result; 
  }

  pos_type asset_streambuf::seekpos(pos_type pos, std::ios_base::openmode which)
  {
    if ((which & (std::ios_base::in | std::ios_base::out)) == std::ios_base::in)
    {
      char* p(eback() + pos);
      if((p >= eback()) && (p < egptr()))
      {
        setg(eback(), p, egptr());
        return pos;
      }
    }
    return pos_type(off_type(-1));
  }
  
  asset_istream::asset_istream(std::string const& filePath, std::ios_base::openmode mode)
   : std::istream(&streambuf)
   , asset(::AAssetManager_open(g_assetManager, filePath.c_str(), AASSET_MODE_BUFFER))
   , streambuf(asset)
  {
    __android_log_print(ANDROID_LOG_INFO, __FUNCTION__, "opened asset file \"%s\": %p", filePath.c_str(), asset);	
  }

  asset_istream::~asset_istream()
  {
    ::AAsset_close(asset);
    __android_log_print(ANDROID_LOG_INFO, __FUNCTION__, "closed asset: %p", asset);	
  }
}

#endif

