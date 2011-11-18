#include <iostream>
#include "thread.hpp"
//#include "opencv2/opencv.hpp"
//#include "opencv2/gpu/gpu.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <list>
#include <cmath>
#include <string>
#include "filesystem.hpp"
#include "Galleries.hpp"
#include "FaceFactor.hpp"
#include <sstream> //do nazw plików

//#include "Lapacz.hpp"

using namespace cv;
using namespace std;

//const float FACE_FACTOR=0.3;


int main(int argc,char **argv){

  Mat img,eq;
  char ster='1';
  Mat mid;
  Mat bw;
  Mat facePics;

  int outWidth=200;
  int outHeight=static_cast<int>(outWidth+FACE_FACTOR*outWidth);

  Size rozm(OUT_WIDTH,OUT_HEIGHT);
  string adres,label;

  Galleries galleries;

  int m,n;

  vector<Rect> faces; 

  VideoCapture cap;

  CascadeClassifier finder;
  int limit=1000;
  int counter=0;

  if(argc<4){
    cerr<<"Error: not enough parameters."<<endl<<argv[0]
	<<" galleries_folder label input_source [number_of_photos_to_extract]"<<endl;
    return 1;
  }

  finder.load("kaskady/haarcascade_frontalface_alt_tree.xml");
  adres=argv[1];
  label=argv[2];
  cap.open(argv[3]);
  if(argc>=5){
    limit=atoi(argv[4]);
  }

  try{
    galleries.setPath(adres);
    galleries.load("galeria.xml");
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in fucntion "<<__func__;
    cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
  }

  // namedWindow("faces",CV_WINDOW_NORMAL);
  // namedWindow("input",CV_WINDOW_NORMAL);
  // namedWindow("equalised",CV_WINDOW_NORMAL);
  
  while(ster!='q' && counter<limit){
    try{
      cap>>img;
      if(img.data==NULL){
	break;
      }
      img.copyTo(facePics);
      
      cvtColor(img,bw,CV_RGB2GRAY);
      equalizeHist(bw,eq);
    }

    catch(Exception ex){
      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	  <<" in fucntion "<<__func__;
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
    }
    
    try{
      
      {
	stringstream sBufor;
	string cel,buff;
	finder.detectMultiScale(eq,faces,1.3);
	if(!faces.empty()){
	  m=floor(sqrt(faces.size()));
	  n=ceil(sqrt(faces.size()));
	  mid.create(rozm.height*m,rozm.width*n,eq.type());
	  int i=0;
	    
	  for(vector<Rect>::iterator it=faces.begin();
	      it!=faces.end();++it,++i){
	    it->y-=(it->height)*FACE_FACTOR/2;
	    it->height*=(1+FACE_FACTOR);
	    //	    cerr<<rozm.width<<" "<<rozm.height<<endl;
	    rectangle(facePics,
		      Point(it->x,it->y),
		      Point(it->x+it->width,it->y+it->height),
		      Scalar(255,0,0));
	    
	    Mat midPt=mid(Rect(rozm.width*(i/n),
			       rozm.height*(i%m),
			       rozm.width,rozm.height));
	    resize(Mat(eq,(*it)),midPt,midPt.size(),0,0,CV_INTER_LINEAR);
	    if(faces.size()==1){
	      galleries.add(label,midPt);
	      cerr<<++counter<<endl;
	    }
	  }
	}
	if(ster!='q'){
	   if(!faces.empty()){
	    // imshow("input",facePics);
	    // imshow("faces",mid);
	  }
	  ster=waitKey(10);
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
    
   galleries.save("galeria.xml");
   
   //koniec zapisu
  
  return 0;
}
