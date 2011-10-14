#include "ocv2pit.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;

ppr_error_type mat2PprImage(cv::Mat& in, 
			    ppr_image_type& out,
			    const ppr_raw_image_color_space_type& colorspace){
  cv::Mat ready;
  ppr_raw_image_type mid;
  
  if(in.channels()!=1&&in.channels()!=3){
    return PPR_INVALID_IMAGE;
  }
  /*
  if(in.channels()==1){
    in.convertTo(ready,CV_8UC1);
    if(colorspace!=PPR_RAW_IMAGE_GRAY8){
      cv::Mat tmp=ready;
      cvtColor(tmp,ready,CV_GRAY2RGB);
      tmp=ready;
      switch(colorspace){
      case PPR_RAW_IMAGE_RGB24:
      case PPR_RAW_IMAGE_BGR24:
	break;
      case PPR_RAW_IMAGE_YUV24:
	cvtColor(tmp,ready,CV_RGB2YUV);
	break;
      case PPR_RAW_IMAGE_ARGB32:
	// Because the alpha channel is unused and Opencv has RGBA/BGRA format
	// implementation of conversion would be pointless
	return PPR_INVALID_IMAGE_COLOR_SPACE;
      }
    }
  }
  */

  switch(colorspace){
  case PPR_RAW_IMAGE_GRAY8:
    if(in.channels()!=1){
      return PPR_INVALID_IMAGE;
    }else{
      in.convertTo(ready,CV_8UC1); //making sure that data type is uchar
    }
    break;
  case PPR_RAW_IMAGE_RGB24:
  case PPR_RAW_IMAGE_BGR24:
    if(in.channels()!=3){
      return PPR_INVALID_IMAGE;
    }else{
      in.convertTo(ready,CV_8UC3); //same as higher
    }
    break;
  case PPR_RAW_IMAGE_YUV:
    if(in.channels()!=3){
      return PPR_INVALID_IMAGE;
    }else{
      Mat tmp;
      in.convertTo(tmp,CV_8UC3);
      cvtColor(tmp,ready,CV_RGB2YUV);
    }
    break;
  default:
    return PPR_INVALID_IMAGE_COLOR_SPACE;
  }
   
  mid.bytes_per_line=ready.cols*ready.channels();
  mid.data=ready.data;
  mid.width=ready.cols;
  mid.height=ready.rows;
  mid.color_space=PPR_RAW_IMAGE_GRAY8;
  
  return ppr_create_image(mid,&out);
}    

