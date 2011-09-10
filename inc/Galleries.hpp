#include <vector>
//#include "Gallery.hpp"
#include <string>
#include "opencv2/core/core.hpp"

#ifndef __GALLERIES__
#define __GALLERIES__

class Galleries{
  struct Gallery{
    string label;
    int counter;
    cv::Size picSize;
    vector<string> photos;
  };
  std::vector<Gallery> gal;
public:
  Galleries(){}
  Galleries(string filename){load(filename);}
  void load(string fileName);
  void save(string fileName);
  void add(string fileName, cv::Mat image);
  int totalSize(){return gal.size();}
  cv::Mat getPicture(string label, int number);
  cv::Mat getPicture(int galleryNumber, int photoNumber);
  int gallerySize(string label);
  int gallerySize(int galleryNumber);
  ~Galleries();

};

#endif
  
