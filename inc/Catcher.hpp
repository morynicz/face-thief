#include "thread.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include <iostream>
#include <string>
#ifndef CATCHER_HPP
#define CATCHER_HPP

using namespace cv;

static const int CATCH_CANNOT_OPEN_DEVICE=-1;
static const int CATCH_CANNOT_OPEN_FILE=-2;

class Catcher{
  cv::Mat fr;
  boost::mutex *mut;
  boost::thread* thr;

  class Camera{
    int frameRate;
    cv::VideoCapture cam;
    cv::Mat *fr;
    boost::mutex *mut;
  public:
      Camera(){ 
	mut=NULL;
      }
    Camera(const int &nr,
	   cv::Mat *mat,
	   boost::mutex *mtx);
    Camera(const std::string &name,
	   cv::Mat *mat,
	   boost::mutex *mtx);
    void  operator()();
    ~Camera();
  };
  Camera cam;
public:
  Catcher();
  void init(const int &nr);
  void init(const std::string &name);
  ~Catcher(void);
  void catchFrame(cv::Mat& frame);
};

#endif
