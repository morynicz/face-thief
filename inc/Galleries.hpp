#include <vector>
//#include "Gallery.hpp"
#include <string>
#include <vector>
#include "opencv2/core/core.hpp"

#ifndef __GALLERIES__
#define __GALLERIES__

using std::string;

const string GALLERIES="Galeries";                                             
const string LABEL="Label";                                                   
const string COUNTER="COUNTER";
const string ADDRES="Addres"; 

const int INCORRECT_GALLERY_NUMBER=1;
const int INCORRECT_PHOTO_NUMBER=2;
const int LABEL_NOT_FOUND=3;
const int CANNOT_OPEN_FILE=4;
const int NO_PATH_DECLARED=5;
const int NO_SUCH_DIRECTORY=6;


class Galleries{
  struct Gallery{
    string label;
    int counter;
    cv::Size picSize;
    std::vector<string> photos;
  };
  std::vector<Gallery> _gal;
  std::string _path;
public:
  Galleries(){_path.clear();}
  Galleries(string path,string filename);
  void load(string filename);
  void save(string filename);
  void add(string label, cv::Mat image);
  int totalSize(){return _gal.size();}
  
  void setPath(string path);

  cv::Mat getPicture(string label, int number);
  cv::Mat getPicture(int galleryNumber, int photoNumber);
  int gallerySize(string label);
  int gallerySize(int galleryNumber);
  ~Galleries(){};
};

#endif
  
