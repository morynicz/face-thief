#include <iostream>
#include "thread.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#include <vector>
#include <list>
#include <cmath>
#include <string>
#include "filesystem.hpp"
#include "Galleries.hpp"
#include "FaceFactor.hpp"

#include <sstream> //do nazw plików

#include "Catcher.hpp"

using namespace cv;
using namespace std;

//const float FACE_FACTOR=0.0;


    

int main(int argc,char **argv){

  Mat img,eq;
  char control='1';
  Mat mid;
  //  Mat zera;

  //  Mat inDft,outDft;
  //  Mat czer;
  Mat bw;
  Mat facePics;

  int outWidth=200;
  int outHeight=outWidth+FACE_FACTOR*outWidth;


  Size picSize(outWidth,outHeight);
  
  string adres,label;

  Galleries galleries;

  int m,n;
  int numerZdjecia,numerGalerii;

  vector<Rect> faces; 

  Catcher cap;
  cap.init(0);

  CascadeClassifier finder;

  finder.load("kaskady/haarcascade_frontalface_alt_tree.xml");
  adres=argv[1];
  label=argv[2];
 
  try{
    galleries.setPath(adres);
    galleries.load("galeria.xml");
  }
 
 catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__;
    cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
  }
  

  namedWindow("in",CV_WINDOW_NORMAL);
  namedWindow("proces",CV_WINDOW_NORMAL);
  namedWindow("test",CV_WINDOW_NORMAL);
  namedWindow("gemba",CV_WINDOW_NORMAL);
  namedWindow("z_galerii",CV_WINDOW_NORMAL);

  
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
	string cel,buff;
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
	    imshow("twarz",midPt);
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
	      cout<<"podaj numer galerii i numer zdjęcia"<<endl;
	      cin>>numerGalerii>>numerZdjecia;
	      Mat zGalerii=galleries.getPicture(numerGalerii,numerZdjecia);
	      cerr<<flush;
	      imshow("z_galerii",zGalerii);
	    
	    }
	  if(!faces.empty()){
	    imshow("test",facePics);
	    imshow("proces",mid);
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
    
  galleries.save("galeria.xml");
  
  //koniec zapisu
  
  return 0;
}
