#include "Gallery.hpp"
#include <boost/filesystem>

Gallery::Gallery(){
  label="unknown";
  counter=0;
}

Gallery::Gallery(string name):label(name){
  counter=0;
}

void Gallery::addPicture(string picName){
  if(boost::exists(picName)){
    photos.push_back(picName);
  }else{
    cv::Exception ex(NO_SUCH_PHOTO,"There is no such photo",
		  __func__,__FILE__,__NAME__);
    throw ex;
  }
}

void Gallery::addPicture(cv::Mat img,string picName){
  Mat toWrite(picSize.width,picSize.height,CV_8U);
  resize(img,toWrite,toWrite.size(),
	 0,0,CV_INTER_LINEAR);
  imwrite(picName,toWrite);

  try{
    addPicture(picName);
  }
  catch(cv::Exception ex){
    if(ex.code == NO_SUCH_FILE){
      cv::Exception ex2(IMAGE_WRITE_UNSUCCESFULL,
		       "Exception: Could not write image",
		       __func__,__FILE__,__NAME__);
    }else{
      cv::Exception ex2=ex;
    }
    throw ex2;
  }
}

cv::Mat getPicture(int number){
  if(number<0 || number > photos.size()){
      cv::Exception ex(BAD_IMAGE_NUMBER,
		       "Exception: Incorrect image number",
		       __func__,__FILE__,__NAME__);
      throw ex;
  }
  Mat img=imread(photos[number]);
  return img;
}

void deletePicture(int number){
  if(number<0 || number > photos.size()){
      cv::Exception ex(BAD_IMAGE_NUMBER,
		       "Exception: Incorrect image number",
		       __func__,__FILE__,__NAME__);
      throw ex;
  }
      
  photos.erase(photos.begin()+number);
}

Gallery::~Gallery(){}
