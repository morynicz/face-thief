#include "thread.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

#ifndef LAPACZ
#define LAPACZ

using namespace cv;

class Lapacz{
  cv::Mat kl;
  boost::mutex *mut;
  boost::thread* wat;

  class Kamera{
    cv::VideoCapture kam;
    cv::Mat kl;
    boost::mutex *mut;
  public:
    Kamera(){ 
      mut=NULL;
    }
    Kamera(int nr,
	   cv::Mat &mat,
	   boost::mutex *mtx);
    void  operator()();
  };

  Kamera kam;
public:
  Lapacz(int nr);
  ~Lapacz();
  void stopKlatka(cv::Mat& klatka);
};

#endif
