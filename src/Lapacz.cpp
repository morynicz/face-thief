#include "Lapacz.hpp"

#include <iostream>

using namespace std;

Lapacz::Kamera::Kamera(const int &nr,
		       cv::Mat *mat,
		       boost::mutex *mtx):kam(nr){
 
  if(!kam.isOpened()){
    cv::Exception err(CATCH_CANNOT_OPEN_DEVICE,
		      "file cannot be opened",
		      __func__,__FILE__,__LINE__);
    throw err;
  }
  kl=mat;
  mut=mtx;
  frameRate=0;
}

Lapacz::Kamera::Kamera(const std::string &name,
		       cv::Mat *mat,
		       boost::mutex *mtx):kam(name){
  
  if(!kam.isOpened()){
    cv::Exception err(CATCH_CANNOT_OPEN_FILE,
		      "file cannot be opened",
		      __func__,__FILE__,__LINE__);
    throw err;
  }
  kl=mat;
  mut=mtx;
  frameRate=kam.get(CV_CAP_PROP_FPS);
  cerr<<frameRate<<endl;
  if(frameRate<=0){
    frameRate=30;
  }
}

Lapacz::Kamera::~Kamera(){
}


void Lapacz::Kamera::operator()(){
  // namedWindow("raw_capture",CV_WINDOW_NORMAL);
  if(frameRate>0){
    int waitTime=1000/frameRate;
    while(!boost::this_thread::interruption_requested()){
      mut->lock();
      //     waitKey(waitTime);
      kam>>(*kl);
      mut->unlock();
    }
  }else{
    while(!boost::this_thread::interruption_requested()){
      mut->lock();
      kam>>(*kl);
      mut->unlock();
    }
  }
}


    
Lapacz::Lapacz(){
  mut=NULL;
  wat=NULL;
}

void Lapacz::init(const int &nr){
  if(wat!=NULL){
    wat->interrupt();
    wat->join();
    delete wat;
    wat=NULL;
  }
  if(mut!=NULL){
    delete mut;
  }
  mut=new boost::mutex;
  kam=Kamera(nr,&kl,mut);
  wat=new boost::thread(boost::ref(kam));
}

void Lapacz::init(const std::string& name){
  if(wat!=NULL){
    wat->interrupt();
    wat->join();
    delete wat;
    wat=NULL;
  }
  if(mut!=NULL){
    delete mut;
  }
  mut=new boost::mutex;
  kam=Kamera(name,&kl,mut);
  wat=new boost::thread(boost::ref(kam));
}


Lapacz::~Lapacz(void){
  if(wat){
    wat->interrupt();
    wat->join();
    delete wat;
  }
  if(mut){
    delete mut;
  }
}

void Lapacz::stopKlatka(cv::Mat& klatka){
  mut->lock();
  klatka=kl.clone();
  mut->unlock();
}

