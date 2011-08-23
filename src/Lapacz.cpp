#include "Lapacz.hpp"


Lapacz::Kamera::Kamera(int nr,
		       cv::Mat &mat,
		       boost::mutex *mtx):kam(nr){
 
  if(!kam.isOpened())
    throw "Nieudane otwarcie kamery";
  
  kl=mat;
  mut=mtx;
}

void Lapacz::Kamera::operator()(){
  while(!boost::this_thread::interruption_requested()){
    mut->lock();
    kam>>kl;
    mut->unlock();
  }
}
    
Lapacz::Lapacz(int nr){
  mut=new boost::mutex;
  kam=Kamera(nr,kl,mut);
  wat=new boost::thread(kam);
}

Lapacz::~Lapacz(){
  wat->interrupt();
  delete mut;
  wat->join();
}

void Lapacz::stopKlatka(cv::Mat& klatka){
  mut->lock();
  klatka=kl.clone();
  mut->unlock();
}

/*
int main(){
  Lapacz lap(1,2);
  return 0;
}
*/
