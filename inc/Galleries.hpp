/// \file
/// \brief Header file for class Galleries
/// \author Michał Orynicz
#include <vector>
#include <string>
#include <vector>
#include "opencv2/core/core.hpp"

#ifndef GALLERIES_HPP
#define GALLERIES_HPP

using std::string;

///\cond

// constant string labels for saving and loading galleries
const string GALLERIES="GALLERIES";                                             
const string LABEL="LABEL";                                                   
const string COUNTER="COUNTER";
const string ADDRESS="ADDRESS"; 
const string SIZE="SIZE";
const string WIDTH="WIDTH";
const string HEIGHT="HEIGHT";
const string TYPE="TYPE";

// Error codes
const int INCORRECT_GALLERY_NUMBER=1;
const int INCORRECT_PHOTO_NUMBER=2;
const int LABEL_NOT_FOUND=3;
const int CANNOT_OPEN_FILE=4;
const int NO_PATH_DECLARED=5;
const int NO_SUCH_DIRECTORY=6;
const int NON_UNIFORM_GALLERY=7;
const int WRONG_PICTURE_SIZE=8;
const int WRONG_PICTURE_TYPE=9;
const int PITTPATT_ERROR=10;
const int UNKNOWN_MAT_TYPE=11;
const int NO_SUCH_FILE=12;
const int SVM_TRAINING_FAILURE=13;
const int INITIAL_SIZE=-2;
///\endcond


///\brief Class containing galleries of pictures
class Galleries{
  ///\brief Class containing gallery of pictures of object described by label
  struct Gallery{
    string label; ///< Label of object
    int counter; ///< Number of pictures in gallery
    std::vector<string> photos; ///< Vector of picture addresses
  };

  ///\brief Size of a picture. 

  /// Size of a picture. All pictures must have the same dimensions
  cv::Size _picSize;

  int _picType; ///< Type of a picture (OpenCV picture type)

  std::vector<Gallery> _gal; ///< Vector containing galleries 

  std::string _path; ///< Path to the directory designed for the galleries

  /// \brief Method for copying picture adress to a gallery
  void addPictureByAddress(string label,string target);

  /// \brief Method for retrieveing picture address from a gallery
  string getPictureAddress(int galleryNumber,int picNr);

public:
  /// \brief Default constructor
  Galleries(){
    _path.clear();
    _picType=0;
    _picSize.width=INITIAL_SIZE;
    _picSize.height=INITIAL_SIZE;
  }
  /// \brief Constructor which sets path and opens a gallery file
  Galleries(string path,string filename);
  void load(string filename); ///< Method loading galleries from a file

  void save(string filename); ///< Method saving galleries to a file

  ///\brief Method adding image to gallery with proper label
  void add(string label, cv::Mat image); 

  ///\brief Method returning the number of galleries
  int totalSize(){return _gal.size();}

  ///\brief Method setting path to the directory designated for galleries
  void setPath(string path);

  ///\brief Method retrieveing a picture from galleries
  cv::Mat getPicture(string label, int number);

  ///\brief Method retrieveing a picture from galleries
  cv::Mat getPicture(int galleryNumber, int photoNumber);

  ///\brief Method retrieveing pictures size 
  
  /// Method retrieveing pictures size. All pictures must have the same size
  cv::Size getPictureSize(){return _picSize;}

  ///\brief Method retrieveing the type of the pictures

  /// Method retrieveing the type of the pictures. All pictures should have 
  /// the same type
  int getPictureType(){return _picType;}

  ///\brief Method returns the size of chosen gallery
  int gallerySize(std::string label); 

  /// \brief Method returns the size of chosen gallery
  int gallerySize(int galleryNumber);

  /// \brief Method returning the label of chosen gallery
  std::string getGalleryLabel(int galleryNumber);

  /// \brief Method returning the number of chosen gallery
  int getGalleryNumber(std::string label);

  /// \brief Method dividing galleries into K subsets
  void createKSubsets(const int &K,const std::string &nameStub,
		      std::vector<string> &galleriesAddresses);

  /// \brief Destructor
  ~Galleries(){};
};

#endif
  
