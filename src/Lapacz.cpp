#include "Lapacz.hpp"

#include <iostream>

using namespace std;

Lapacz::Kamera::Kamera(int nr,
		       cv::Mat *mat,
		       boost::mutex *mtx):kam(nr){
 
  if(!kam.isOpened())
    throw "Nieudane otwarcie kamery";
  
  kl=mat;
  mut=mtx;
}

void Lapacz::Kamera::operator()(){
  // namedWindow("raw_capture",CV_WINDOW_NORMAL);
  while(!boost::this_thread::interruption_requested()){
    mut->lock();
    kam>>(*kl);
    //    imshow("raw_capture",(*kl));
    // waitKey(100);
    mut->unlock();
  }
}
    
Lapacz::Lapacz(int nr){
  mut=new boost::mutex;
  kam=Kamera(nr,&kl,mut);
  wat=new boost::thread(boost::ref(kam));
}

Lapacz::~Lapacz(){
  wat->interrupt();
  wat->join();
  delete mut;
}

void Lapacz::stopKlatka(cv::Mat& klatka){
  mut->lock();
  klatka=kl.clone();
  mut->unlock();
}

