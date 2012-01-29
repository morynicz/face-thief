///\file
/// \brief Main function file for program building gallery from single photos
///\author Michał Orynicz

#include <iostream>
#include <boost/thread.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <vector>
#include <list>
#include <cmath>
#include <string>
#include <boost/filesystem.hpp>
#include "Galleries.hpp"
#include "FaceFactor.hpp"

#include <sstream> //do nazw plików

#include "Catcher.hpp"

using namespace cv;
using namespace std;

   
/// This program can create gallery from single photos
int main(int argc,char **argv){

  Mat img,eq;
  char control='1';
  Mat mid;
  Mat bw;
  Mat facePics;

  Size picSize(OUT_WIDTH,OUT_HEIGHT);
  
  string adres,label;

  Galleries galleries;

  int m,n;
  int photoNumber,galleryNumber;

  vector<Rect> faces; 

  Catcher cap;
  cap.init(0);

  CascadeClassifier finder;

  finder.load("kaskady/haarcascade_frontalface_alt_tree.xml");

  if(argc<3){
    cerr<<" Error: incorrect number of arguments. Correct invocation:"<<endl
	<<argv[0]<<" galleries_folder gallery_label"<<endl;
    return 1;
  }

  adres=argv[1];
  label=argv[2];
 
  try{
    galleries.setPath(adres);
    galleries.load("gallery.xml");
  }
 
 catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__;
    cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
  }
  

  namedWindow("in",CV_WINDOW_NORMAL);
  namedWindow("process",CV_WINDOW_NORMAL);
  namedWindow("test",CV_WINDOW_NORMAL);
  namedWindow("face",CV_WINDOW_NORMAL);
  namedWindow("fromGallery",CV_WINDOW_NORMAL);

  
   while(control!='q'){
    try{
      cap.catchFrame(img);
      img.copyTo(facePics);
      cvtColor(img,bw,CV_RGB2GRAY);
      equalizeHist(bw,eq);
      imshow("in",img);
    }

    catch(Exception ex){
      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	  <<" in fucntion "<<__func__;
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
    }
    
    try{
      
      {
	stringstream sBufor;
	string target,buff;
	finder.detectMultiScale(eq,faces,1.3);
	if(!faces.empty()){
	  m=floor(sqrt(faces.size()));
	  n=ceil(sqrt(faces.size()));
	  mid.create(picSize.height*m,picSize.width*n,eq.type());
	  int i=0;
	  
	  for(vector<Rect>::iterator it=faces.begin();
	      it!=faces.end();++it,++i){
	    it->y-=(it->height)*FACE_FACTOR/2;
	    it->height*=(1+FACE_FACTOR);
	    rectangle(facePics,
		      Point(it->x,it->y),
		      Point(it->x+it->width,it->y+it->height),
		      Scalar(255,0,0));
	    
	    Mat midPt=mid(Rect(picSize.width*(i/n),
			       picSize.height*(i%m),
			       picSize.width,picSize.height));
	    resize(Mat(eq,(*it)),midPt,midPt.size(),0,0,CV_INTER_LINEAR);
	    imshow("face",midPt);
	    if(control!='q'){
	      control=' ';
	      control=waitKey(1000);
	      if(control=='c'){
		galleries.add(label,midPt);
		//=================
	      }
	    }
	  }
	}
	if(control!='q'){
	  if(control=='w'){
	      cout<<"gallery_number photo_number:"<<endl;
	      cin>>galleryNumber>>photoNumber;
	      Mat fromGallery=galleries.getPicture(galleryNumber,photoNumber);
	      cerr<<flush;
	      imshow("fromGallery",fromGallery);
	    
	    }
	  if(!faces.empty()){
	    imshow("test",facePics);
	    imshow("process",mid);
	  }
	  control=waitKey(100);
	}
      }
    }
    catch(Exception ex){
      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	  <<" in fucntion "<<__func__<<endl;
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
    }
    
  }
  
    //===========zapis
    
  galleries.save("galleries.xml");
  
  //koniec zapisu
  
  return 0;
}
