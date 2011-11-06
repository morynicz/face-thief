#include "Catcher.hpp"

#include <iostream>

using namespace std;

Catcher::Camera::Camera(const int &nr,
		       cv::Mat *mat,
		       boost::mutex *mtx):cam(nr){
 
  if(!cam.isOpened()){
    cv::Exception err(CATCH_CANNOT_OPEN_DEVICE,
		      "file cannot be opened",
		      __func__,__FILE__,__LINE__);
    throw err;
  }
  fr=mat;
  mut=mtx;
  frameRate=0;
}

Catcher::Camera::Camera(const std::string &name,
		       cv::Mat *mat,
		       boost::mutex *mtx):cam(name){
  
  if(!cam.isOpened()){
    cv::Exception err(CATCH_CANNOT_OPEN_FILE,
		      "file cannot be opened",
		      __func__,__FILE__,__LINE__);
    throw err;
  }
  fr=mat;
  mut=mtx;
  frameRate=cam.get(CV_CAP_PROP_FPS);
  cerr<<frameRate<<endl;
  if(frameRate<=0){
    frameRate=30;
  }
}

Catcher::Camera::~Camera(){
}


void Catcher::Camera::operator()(){
  // namedWindow("raw_capture",CV_WINDOW_NORMAL);
  if(frameRate>0){
    //   int waitTime=1000/frameRate;
    while(!boost::this_thread::interruption_requested()){
      mut->lock();
      //     waitKey(waitTime);
      cam>>(*fr);
      mut->unlock();
    }
  }else{
    while(!boost::this_thread::interruption_requested()){
      mut->lock();
      cam>>(*fr);
      mut->unlock();
    }
  }
}


    
Catcher::Catcher(){
  mut=NULL;
  thr=NULL;
}

void Catcher::init(const int &nr){
  if(thr!=NULL){
    thr->interrupt();
    thr->join();
    delete thr;
    thr=NULL;
  }
  if(mut!=NULL){
    delete mut;
  }
  mut=new boost::mutex;
  cam=Camera(nr,&fr,mut);
  thr=new boost::thread(boost::ref(cam));
}

void Catcher::init(const std::string& name){
  if(thr!=NULL){
    thr->interrupt();
    thr->join();
    delete thr;
    thr=NULL;
  }
  if(mut!=NULL){
    delete mut;
  }
  mut=new boost::mutex;
  cam=Camera(name,&fr,mut);
  thr=new boost::thread(boost::ref(cam));
}


Catcher::~Catcher(void){
  if(thr){
    thr->interrupt();
    thr->join();
    delete thr;
  }
  if(mut){
    delete mut;
  }
}

void Catcher::catchFrame(cv::Mat& frame){
  mut->lock();
  frame=fr.clone();
  mut->unlock();
}

