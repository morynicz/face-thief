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

#include <sstream> //do nazw plików

#include "Lapacz.hpp"

using namespace cv;
using namespace std;

const float FACE_FACTOR=0.2;

  
    

int main(int argc,char **argv){

  Size rozm;
  Mat obr,eq;
  char ster='1';
  Mat mid;
  Mat zera;

  Mat inDft,outDft;
  Mat czer;
  Mat bw;
  Mat gemben;
  Mat ggemben;
  
  string adres;

  //  list<Gallery> galeries; 
  Galleries galleries;
  long long counter=1;

  int m,n;
  int numerZdjecia,numerGalerii;

  vector<Rect> twarze; 
  
  Lapacz kam(0);

  CascadeClassifier szukacz;
  
  szukacz.load(argv[1]);
  adres=argv[2];

  // wczytywanie galerii zdjęć
  try{
    galleries.setPath(adres);
    galleries.load("galeria.xml");
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in fucntion "<<__func__;
    cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
  }
  
  cout<<"hi thar"<<endl;
  namedWindow("in",CV_WINDOW_NORMAL);
  namedWindow("proces",CV_WINDOW_NORMAL);
  namedWindow("test",CV_WINDOW_NORMAL);
  namedWindow("gemba",CV_WINDOW_NORMAL);
  namedWindow("z_galerii",CV_WINDOW_NORMAL);
 

  Mat input=galleries.getPicture(0,0);
  cerr<<input.type()<<endl;
  cerr<<CV_8UC3<<endl;
  cerr<<input.channels()<<endl;
  cerr<<input.depth()<<endl;
  imshow("in",input);
  waitKey(500);














  /*
   while(ster!='q'){
    try{
      kam.stopKlatka(obr);
      obr.copyTo(gemben);
      rozm=obr.size();
      cvtColor(obr,bw,CV_RGB2GRAY);
      equalizeHist(bw,eq);
      imshow("in",obr);
    }

    catch(Exception ex){
      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	  <<" in fucntion "<<__func__;
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
    }
    
    try{
      
      {
	stringstream sBufor;
	string cel,label,buff;
	szukacz.detectMultiScale(eq,twarze,1.3);
	if(!twarze.empty()){
	  m=floor(sqrt(twarze.size()));
	  n=ceil(sqrt(twarze.size()));
	  mid.create(rozm.width*m*(1+FACE_FACTOR),rozm.width*n,CV_8UC3);
	  int i=0;
	  
	  //	  list<Gallery>::iterator git;
	  
	  for(vector<Rect>::iterator it=twarze.begin();
	      it!=twarze.end();++it,++i){
	    it->y-=(it->height)*FACE_FACTOR/2;
	    it->height*=(1+FACE_FACTOR);
	    rectangle(gemben,
		      Point(it->x,it->y),
		      Point(it->x+it->width,it->y+it->height),
		      Scalar(255,0,0));
	    
	    Mat midPt=mid(Rect(rozm.width*(i/n),
			       rozm.width*(i%m)*(1+FACE_FACTOR),
			       rozm.width,rozm.width*(1+FACE_FACTOR)));
	    resize(Mat(obr,(*it)),midPt,midPt.size(),0,0,CV_INTER_LINEAR);
	    
	    
	    
	    imshow("gemba",midPt);
	    if(ster!='q'){
	      ster=' ';
	      ster=waitKey(1000);
	      if(ster=='c'){
		cout<<"kto to?"<<endl;
		cin>>label;
		if(label=="koniec")
		  break;
		galleries.add(label,midPt);
		//=================
	      }
	      if(label=="koniec")
		break;
	    }
	  }
	}
	if(ster!='q'){
	  if(ster=='w'){
	      cout<<"podaj numer galerii i numer zdjęcia"<<endl;
	      cin>>numerGalerii>>numerZdjecia;
	      Mat zGalerii=galleries.getPicture(numerGalerii,numerZdjecia);
	      cerr<<flush;
	      imshow("z_galerii",zGalerii);
	    
	    }
	  if(!twarze.empty()){
	    imshow("test",gemben);
	    imshow("proces",mid);
	  }
	  ster=waitKey(100);
	}
      }
    }
    catch(Exception ex){
      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	  <<" in fucntion "<<__func__<<endl;
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
    }
    
  }
  */
    //===========zapis
    
  galleries.save("galeria.xml");
  
  //koniec zapisu
  
  return 0;
}
