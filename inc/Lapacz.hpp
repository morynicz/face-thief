#include "thread.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include <iostream>
#include <string>
#ifndef LAPACZ_HPP
#define LAPACZ_HPP

using namespace cv;

static const int CATCH_CANNOT_OPEN_DEVICE=-1;
static const int CATCH_CANNOT_OPEN_FILE=-2;

class Lapacz{
  cv::Mat kl;
  boost::mutex *mut;
  boost::thread* wat;

  class Kamera{
    int frameRate;
    cv::VideoCapture kam;
    cv::Mat *kl;
    boost::mutex *mut;
  public:
      Kamera(){ 
	mut=NULL;
      }
    Kamera(const int &nr,
	   cv::Mat *mat,
	   boost::mutex *mtx);
    Kamera(const std::string &name,
	   cv::Mat *mat,
	   boost::mutex *mtx);
    void  operator()();
    ~Kamera();
  };
  Kamera kam;
public:
  Lapacz();
  void init(const int &nr);
  void init(const std::string &name);
  ~Lapacz(void);
  void stopKlatka(cv::Mat& klatka);
};

#endif
