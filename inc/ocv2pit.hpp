#include "opencv2/core/core.hpp"
#include <pittpatt_ftr_sdk.h>
//#include "pittpatt_license.h"

/// Function converting OpenCV Mat image to PittPatt ppr_image_type image 
ppr_error_type mat2PprImage(cv::Mat& in,
			    ppr_image_type& out,
			    const ppr_raw_image_color_space_type& colorspace);

/// Function converting PittPatt ppr_image_type image to Opencv Mat type image;
//will be not, because pittpatt doesn't give a chance to do it.
//cv::Mat pprImage2Mat(const ppr_image& in);

  
