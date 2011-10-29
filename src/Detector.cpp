#include <iostream>
#include "thread.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <list>
#include <cmath>
#include <string>
#include "filesystem.hpp"
#include "timer.hpp"
#include "Galleries.hpp"
#include "Rec.hpp"
#include "PCARec.hpp"
#include "SVMRec.hpp"

#include "PittpattPresence.hpp"
#ifdef PITTPATT_PRESENT
#include "PPRec.hpp"

#define PPREC_PRECOMPUTED
#endif
#include <sstream> //do nazw plików

#include "Lapacz.hpp"

#define PCAREC
#define PCAREC_PRECOMPUTED
#define SVMREC
//#define SVMREC_PRECOMPUTED

using namespace cv;
using namespace std;

const float FACE_FACTOR=0.3;


void timeElapsed(const boost::timer &time){
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
      <<fmod(time.elapsed(),60) <<"s"<<endl;
}

int main(int argc,char **argv){


  Mat obr,eq;
  //  char ster='1';
  Mat mid;
  Mat bw;
  Mat gemben;

  int outWidth=200;
  int outHeight=outWidth*(1+FACE_FACTOR);

  string zdjecie;

  Size rozm(outWidth,outHeight);
  
  string adres;

  Galleries galleries;
  //long counter=1;

  vector<Rect> twarze; 
  vector<Rec> algorithms;


  CascadeClassifier szukacz;
  
  szukacz.load("kaskady/haarcascade_frontalface_alt_tree.xml");
  if(argc!=2){
    cerr<<"Error: incorrect number of arguments. Correct invocation:"<<endl
	<<argv[0]<<" galleries_folder"<<endl;
    return 1;
  }

  Lapacz kam(0);

  adres=argv[1];
  //  zdjecie=argv[2];

  Mat do_golenia=imread(zdjecie);
  Mat kompresowany;

  // wczytywanie galerii zdjęć
  try{
    galleries.setPath(adres);
    galleries.load("galeria.xml");
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__;
    cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
  }
  
  cout<<"hi thar"<<endl;

  
  boost::timer time;
  {
        
#ifdef PITTPATT_PRESENT
      cout<<"PPR initialisng"<<endl;
      time.restart();
      PPRec pp;
      cout<<"PPR initialised"<<endl;
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	  <<fmod(time.elapsed(),60) <<"s"<<endl;
#ifndef PPREC_PRECOMPUTED
      cout<<"PPR galleries loading"<<endl;
      time.restart();
      pp.loadGalleries(galleries);
      cout<<"PPR galleries loaded"<<endl;
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
      	  <<fmod(time.elapsed(),60) <<"s"<<endl;

      cout<<"PPR computing"<<endl;
       time.restart();
      pp.compute();
      cout<<"PPR computed"<<endl;
      
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
      	   <<fmod(time.elapsed(),60) <<"s"<<endl;
      time.restart();
      cout<<"PPR saving precomputed galleries"<<endl;
      	  pp.savePrecomputedGalleries("PPGallery.ppr");
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
      	  <<fmod(time.elapsed(),60) <<"s"<<endl;

#elseif
      cout<<"PPR loading precomputed galleries"<<endl;
      time.restart();
      pp.loadPrecomputedGalleries("PPGallery.ppr");
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	  <<fmod(time.elapsed(),60) <<"s"<<endl;
#endif   
#endif
 
#ifdef PCAREC
    cout<<"Starting PCA"<<endl;
    PCARec pca;
#ifndef PCAREC_PRECOMPUTED
    cout<<"finished in "<<time.elapsed()<<"s"<<endl;
    cout<<"PCA: Loading galleries"<<endl;
    pca.loadGalleries(galleries);
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
     
    cout<<"PCA: Computing"<<endl;
    time.restart();
    pca.compute();
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;


    cout<<"PCA: saving"<<endl;
    time.restart();
    pca.savePrecomputedGalleries("PCAdata/PCA.xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
#else
    cout<<"PCA: Loading precomputed galleries"<<endl;
    time.restart();
    pca.loadPrecomputedGalleries("PCAdata/PCA.xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;

#endif
#endif
   
#ifdef SVMREC
    cerr<<"SVM starting"<<endl;
    time.restart();
    SVMRec svm;
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;

#ifndef SVMREC_PRECOMPUTED
    cerr<<"SVM: loading galleries"<<endl;
    time.restart();
    svm.loadGalleries(galleries);
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
    
    cerr<<"SVM: computing"<<endl;
    time.restart();
    svm.compute();
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;

    cout<<"SVM: saving"<<endl;
    time.restart();
    svm.savePrecomputedGalleries("SVMdata/SVM.xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
#else    
    cout<<"SVM: Loading precomputed galleries"<<endl;
    time.restart();
    svm.loadPrecomputedGalleries("SVMdata/SVM.xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
#endif
#endif
    {//Control loop
      Mat obr;
      int m,n;
      char control='u';
      namedWindow("skanowane",CV_WINDOW_NORMAL);
      namedWindow("in",CV_WINDOW_NORMAL);
      while(control!='q'){
       
	try{
	  kam.stopKlatka(obr);
	  obr.copyTo(gemben);
	  cvtColor(obr,bw,CV_RGB2GRAY);
	  equalizeHist(bw,eq);
	  imshow("in",obr);
	}
	catch(Exception ex){
	  cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	      <<" in function "<<__func__<<endl;
	  throw ex;
	}   
	try{
	  szukacz.detectMultiScale(eq,twarze,1.3);
	  if(!twarze.empty()){
	    m=floor(sqrt(twarze.size()));
	    n=ceil(sqrt(twarze.size()));
	    mid.create(rozm.width*m*(1+FACE_FACTOR),rozm.width*n,eq.type());
	    int i=0;
	 
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
		resize(Mat(eq,(*it)),midPt,midPt.size(),0,0,CV_INTER_LINEAR);
		imshow("gemba",midPt);
		
		
		
#ifdef SVMREC
		try{
		  string posLabel;
		  cerr<<"SVM: recognising"<<endl;
		  time.restart();
		  
		  
		  std::list<Result> wyniki=svm.recognise(midPt);
		  cout<<"finished in "<<time.elapsed()<<"s"<<endl;
		  for(std::list<Result>::iterator sit=wyniki.begin();
		      sit!=wyniki.end();++sit){
		    cout<<galleries.getGalleryLabel(sit->label)<<" "<<sit->mean
			<<" "<<sit->max<<" "<<sit->min<<endl;
		    if(sit->mean>0)
		      posLabel=posLabel+" "+galleries.getGalleryLabel(sit->label);
		  }		  

		  if(posLabel.size()==0){
		    posLabel="unknown";
		  }

		  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
		      <<fmod(time.elapsed(),60) <<"s"<<endl;

		  putText(gemben,"SVM: "+posLabel,Point(it->x,it->y+it->height+8)
			  ,FONT_HERSHEY_SIMPLEX,
			  1,Scalar(0,0,255));
		}
		catch(Exception ex){
		  cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
		      <<" in function "<<__func__<<endl;
		  throw ex;
		}
#endif		
#ifdef PCAREC
		cout<<"PCA: recognising"<<endl;
		{
		  string minLabel="unknown";
		  double minValue=10;
	     
		  time.restart();
		  std::list<Result> wyniki=pca.recognise(midPt);
		  cout<<"finished in "<<time.elapsed()<<"s"<<endl;
		  for(std::list<Result>::iterator sit=wyniki.begin();
			   sit!=wyniki.end();++sit){
			 cout<<galleries.getGalleryLabel(sit->label)<<" "<<sit->mean
			     <<" "<<sit->max<<" "<<sit->min<<endl;
			 if(sit->mean<minValue){
			   minValue=sit->mean;
			   minLabel=galleries.getGalleryLabel(sit->label);
			 }
		  }
	   
		  putText(gemben,"PCA: "+minLabel,
			  Point(it->x,it->y+it->height+16),
			  FONT_HERSHEY_SIMPLEX,
			  1,Scalar(0,255,0));
		  
		  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
		      <<fmod(time.elapsed(),60) <<"s"<<endl;
		}
#endif
#ifdef PITTPATT_PRESENT
		{
		  string bestMatch;
		  cout<<"PPR recognising"<<endl;
		  time.restart();
		  std::list<Result> wyniki=pp.recognise(midPt);
		  cout<<"PPR recognised "<<endl;
		  for(std::list<Result>::iterator sit=wyniki.begin();
		      sit!=wyniki.end();++sit){
		    cout<<galleries.getGalleryLabel(sit->label)<<" "<<sit->mean
			<<" "<<sit->max<<" "<<sit->min<<endl;
		  }
		  bestMatch=wyniki.front().label;
		    
      
		  putText(gemben,"PPR: "+bestMatch,
			  Point(it->x,it->y+it->height+32),
			  FONT_HERSHEY_SIMPLEX,
			  1,Scalar(255,0,0));
		  
		  
		  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
		      <<fmod(time.elapsed(),60) <<"s"<<endl;
		}
#endif
		imshow("skanowane",gemben);
		
	    }
	  }
	}
	catch(Exception ex){
	  cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	      <<" in function "<<__func__;
	  cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
	}
      
     	control=waitKey(100);
	
      }
    }
  }
  return 0;
}
    
