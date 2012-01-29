/// \file
/// \brief Header file for class Cather
/// \author Michał Orynicz

#include <boost/thread.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#ifndef CATCHER_HPP
#define CATCHER_HPP

using namespace cv;

///\brief Error code for inability to open a device
static const int CATCH_CANNOT_OPEN_DEVICE=-1;
///\brief Error code for inability to open a fil
static const int CATCH_CANNOT_OPEN_FILE=-2;

///\brief Class providing ability to catch the most recent frame from 
/// video stream 

class Catcher{
  cv::Mat fr; ///< Most recent frame
  boost::mutex *mut; ///< Pointer to mutex
  boost::thread* thr; ///< pointer to thread

  ///\brief Class used to ensure, that video streams buffer is empty
  class Camera{
    int frameRate; ///< Number of frames per second
    cv::VideoCapture cam; ///< video stream
    cv::Mat *fr; ///< Most recent frame
    boost::mutex *mut; ///< Mutex
  public:
    ///\brief Constructor, initializes mutex pointer
      Camera(){ 
	mut=NULL;
      }
    ///\brief Constructor for video streams from a device
    Camera(const int &nr,
	   cv::Mat *mat,
	   boost::mutex *mtx);
    ///\brief Constructor for video streams from a video file
    Camera(const std::string &name,
	   cv::Mat *mat,
	   boost::mutex *mtx);
    ///\brief Function used to start emptying of the buffer
    void  operator()();
    ///\brief Destructor
    ~Camera();
  };
  Camera cam; ///< Object providing the most recent frame
public:
  ///\brief Constructor initialising pointer fields
  Catcher();
  ///\brief initialization method for streams from a device
  void init(const int &nr);
  ///\brief initialization method for streams from a file
  void init(const std::string &name);
  ///\brief Destructor removing mutex and thread objects
  ~Catcher();
  ///\brief Method for retrieving the most recent frame
  void catchFrame(cv::Mat& frame);
};

#endif
