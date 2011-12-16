///\file
///\brief File containing implementation of class Catcher
///\author Michał Orynicz
#include "Catcher.hpp"

#include <iostream>

using namespace std;

/// Constructor for video streams originating from a device
/// \param nr - number of the device
/// \param mat - pointer to cv::Mat object which will contain the most recent
/// frame
/// \param mtx - pointer to mutex 
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

/// Constructor for video streams originating from a device
/// \param name - name of video file
/// \param mat - pointer to cv::Mat object which will contain the most recent
/// frame
/// \param mtx - pointer to mutex 
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

/// Empty destructor
Catcher::Camera::~Camera(){
}

/// Method which reads subsequent frames from stream. Ensures that frame 
/// buffer of stream is empty
void Catcher::Camera::operator()(){
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

/// Initializes mut and thr pointers
Catcher::Catcher(){
  mut=NULL;
  thr=NULL;
}

/// Method initializing the object for drawing video stream from a device 
/// with given number
/// \param nr - number of device from which the video stream will be drawn
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

/// Method initializing the object for drawing video stream from a video file
/// \param name - name of file from which the video stream will be read
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

/// Destructor, ensures that all threads and dynamic objects will be 
/// disposed properly
Catcher::~Catcher(){
  if(thr){
    thr->interrupt();
    thr->join();
    delete thr;
  }
  if(mut){
    delete mut;
  }
}

/// Method which makes a deep copy of most recent frame, and returns it outside
/// \param frame - object in which the deep copy of most recent frame will 
/// be placed
void Catcher::catchFrame(cv::Mat& frame){
  mut->lock();
  frame=fr.clone();
  mut->unlock();
}

