#include <vector>
#include <string>
#include "opencv2/core/core.hpp"

#ifndef __GALLERY__
#define __GALLERY__ 

class Gallery{
  string label;
  int counter;
  cv::Size picSize;
  vector<string> photos;
public:
  Gallery();
  Gallery(string name);
  
  int getSize(){return photos.size();}
  int getCounter(){return counter;}
  string getLabel(){return label;}
  vector<string>& getPhotos(){return photos;}
  cv::getPicSize(){return picSize;}

  void setCounter(int count){counter=count;}
  void setLabel(int name){label=name;}
  
  void addPicture(cv::Mat img,string picName);
  void addPicture(string path);
 
  cv::Mat getPicture(int number);
  
  void deletePicture(int number);
  
  ~Gallery();

  const int IMAGE_WRITE_UNSUCCESFULL=1;
  const int NO_SUCH_FILE=2;
};

#endif
