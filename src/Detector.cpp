///\file
///\brief Main file for program recognizing faces from video stream

#include <iostream>
#include "thread.hpp"
// #include "opencv2/opencv.hpp"
// #include "opencv2/gpu/gpu.hpp"
// #include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <list>
#include <cmath>
#include <string>
#include <sstream>
#include "filesystem.hpp"
#include "timer.hpp"
#include "Galleries.hpp"
#include "Rec.hpp"
#include "PCARec.hpp"
#include "SVMRec.hpp"
#include "FaceFactor.hpp"

#include "PittpattPresence.hpp"
#ifdef PITTPATT_PRESENT
#include "PPRec.hpp"

#define PPREC_PRECOMPUTED
#endif
#include <sstream> //do nazw plików

#include "Catcher.hpp"
 
#define PCAREC
#define PCAREC_PRECOMPUTED
#define SVMREC
#define SVMREC_PRECOMPUTED

using namespace cv;
using namespace std;

//const float FACE_FACTOR=0.3;


void timeElapsed(const boost::timer &time){
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
      <<fmod(time.elapsed(),60) <<"s"<<endl;
}

int main(int argc,char **argv){


  Mat eq;
  Mat mid;
  Mat bw;
  Mat gemben;

  int outWidth=200;
  int outHeight=outWidth+FACE_FACTOR*outWidth;

  string zdjecie;

  Size rozm(OUT_WIDTH,OUT_HEIGHT);
  
  string adres;

  Galleries galleries;
  //long counter=1;

  vector<Rect> twarze; 
  vector<Rec*> alg;


  CascadeClassifier szukacz;
   
  szukacz.load("kaskady/haarcascade_frontalface_alt_tree.xml");
  if(argc<3){
    cerr<<"Error: incorrect number of arguments. Correct invocation:"<<endl
	<<argv[0]<<" galleries_folder [-c capture_device_nr|-v video_file.avi]"
	<<endl;
    return 1;
  }
  string option=argv[2];
  Catcher kam;
  VideoCapture vid;
  int vc;
  // return 0;
 
  adres=argv[1];

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
      //      PPRec pp;
      alg.push_back(new PPRec);
      alg.back()->initialize();
      cout<<"PPR initialised"<<endl;
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	  <<fmod(time.elapsed(),60) <<"s"<<endl;
#ifndef PPREC_PRECOMPUTED
      cout<<"PPR galleries loading"<<endl;
      time.restart();
      alg.back()->loadGalleries(galleries);
      cout<<"PPR galleries loaded"<<endl;
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
      	  <<fmod(time.elapsed(),60) <<"s"<<endl;

      cout<<"PPR computing"<<endl;
       time.restart();
      alg.back()->compute();
      cout<<"PPR computed"<<endl;
      
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
      	   <<fmod(time.elapsed(),60) <<"s"<<endl;
      time.restart();
      cout<<"PPR saving precomputed galleries"<<endl;
      	  alg.back()->savePrecomputedGalleries((alg.back()->getName()+"Data")
				       +"/"+alg.back()->getName()+".xml");
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
      	  <<fmod(time.elapsed(),60) <<"s"<<endl;

#elseif
      cout<<"PPR loading precomputed galleries"<<endl;
      time.restart();
      alg.back()->loadPrecomputedGalleries((alg.back()->getName()+"Data")
				       +"/"+alg.back()->getName()+".xml");
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	  <<fmod(time.elapsed(),60) <<"s"<<endl;
#endif   

#endif
 
#ifdef PCAREC
    cout<<"Starting PCA"<<endl;
    //PCARec pca;
    alg.push_back(new PCARec);
    alg.back()->initialize();
    cout<<"finished in "<<time.elapsed()<<"s"<<endl;
#ifndef PCAREC_PRECOMPUTED

    cout<<"PCA: Loading galleries"<<endl;
    alg.back()->loadGalleries(galleries);
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
     
    cout<<"PCA: Computing"<<endl;
    time.restart();
    alg.back()->compute();
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;


    cout<<"PCA: saving"<<endl;
    time.restart();
    alg.back()->savePrecomputedGalleries((alg.back()->getName()+"Data")
				       +"/"+alg.back()->getName()+".xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
#else
    cout<<"PCA: Loading precomputed galleries"<<endl;
    time.restart();
    alg.back()->loadPrecomputedGalleries((alg.back()->getName()+"Data")
				       +"/"+alg.back()->getName()+".xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;

#endif
#endif
   
#ifdef SVMREC
    cerr<<"SVM starting"<<endl;
    time.restart();
    //SVMRec svm;
    alg.push_back(new SVMRec);
    alg.back()->initialize();
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;

#ifndef SVMREC_PRECOMPUTED
    cerr<<"SVM: loading galleries"<<endl;
    time.restart();
    alg.back()->loadGalleries(galleries);
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
    
    cerr<<"SVM: computing"<<endl;
    time.restart();
    alg.back()->compute();
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;

    cout<<"SVM: saving"<<endl;
    time.restart();
    alg.back()->savePrecomputedGalleries((alg.back()->getName()+"Data")
				       +"/"+alg.back()->getName()+".xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
#else    
    cout<<"SVM: Loading precomputed galleries"<<endl;
    time.restart();
    alg.back()->loadPrecomputedGalleries((alg.back()->getName()+"Data")
				       +"/"+alg.back()->getName()+".xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
#endif

    try{
      if(option=="-c"){
	stringstream buff;
	int devNr;
	buff<<argv[3];
	buff>>devNr;
	kam.init(devNr);
	vc=-1;
      }else if(option=="-v"){
	//kam.init(argv[3]);
	vid.open(argv[3]);
	if(!vid.isOpened()){
	  cv::Exception err(CANNOT_OPEN_FILE,
			    "file cannot be opened",
			    __func__,__FILE__,__LINE__);
	  throw err;
	}
	vc=1;
      }else{
	cerr<<"invalid option "<<option<<endl;
	return 1;
      }
    }
    catch(Exception ex){
      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	  <<" in function "<<__func__<<endl;
      throw ex;
    }   



#endif
    {//Control loop
      Mat obr;
      int m,n;
      char control='u';
      namedWindow("skanowane",CV_WINDOW_NORMAL);
      namedWindow("in",CV_WINDOW_NORMAL);
      while(control!='q'){
       
	try{
	  if(-1==vc){
	  kam.catchFrame(obr);
	  }else if(1==vc){
	    vid>>obr;
	  }else{
	    return 1;
	  }
	  if(obr.empty()){
	    //  cerr<<"papa"<<endl;
	    return 0;
	  }
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

		{
		  Mat temp=midPt.clone();
		  string bestMatch;
		  for(int z=0;z<alg.size();++z){
		    cout<<alg[z]->getName()<<" recognising"<<endl;
		    time.restart();
		    std::list<Result> wyniki=alg[z]->recognise(temp);
		    cout<<alg[z]->getName()<<" recognised "<<endl;
		    for(std::list<Result>::iterator sit=wyniki.begin();
			sit!=wyniki.end();++sit){
		      cout<<galleries.getGalleryLabel(sit->label)<<" "
			  // <<sit->mean
			  // <<" "<<sit->max<<" "<<sit->min<<endl;
			  <<sit->score<<endl;
		    }
		    bestMatch=galleries.getGalleryLabel(wyniki.front().label);
		    cerr<<endl<<alg[z]->getName()+" "+bestMatch
			<<endl<<endl;
		    
		    putText(gemben,alg[z]->getName()+" "+bestMatch,
			    Point(it->x,it->y+it->height+(z*2+2)*15),
			    FONT_HERSHEY_SIMPLEX,
			    1,Scalar(0,255,0),2);
		    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
			<<fmod(time.elapsed(),60) <<"s"<<endl;
		  }
		}
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
    
		
		
		
// #ifdef SVMREC
// 		try{
// 		  string posLabel;
// 		  cerr<<"SVM: recognising"<<endl;
// 		  time.restart();
		  
		  
// 		  std::list<Result> wyniki=svm.recognise(midPt);
// 		  cout<<"finished in "<<time.elapsed()<<"s"<<endl;
// 		  for(std::list<Result>::iterator sit=wyniki.begin();
// 		      sit!=wyniki.end();++sit){
// 		    cout<<galleries.getGalleryLabel(sit->label)<<" "<<sit->mean
// 			<<" "<<sit->max<<" "<<sit->min<<endl;
// 		    if(sit->mean>0)
// 		      posLabel=posLabel+" "+galleries.getGalleryLabel(sit->label);
// 		  }		  

// 		  if(posLabel.size()==0){
// 		    posLabel="unknown";
// 		  }

// 		  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 		      <<fmod(time.elapsed(),60) <<"s"<<endl;

// 		  putText(gemben,"SVM: "+posLabel,Point(it->x,it->y+it->height+8)
// 			  ,FONT_HERSHEY_SIMPLEX,
// 			  1,Scalar(0,0,255));
// 		}
// 		catch(Exception ex){
// 		  cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
// 		      <<" in function "<<__func__<<endl;
// 		  throw ex;
// 		}
// #endif		
// #ifdef PCAREC
// 		cout<<"PCA: recognising"<<endl;
// 		{
// 		  string minLabel="unknown";
// 		  double minValue=10;
	     
// 		  time.restart();
// 		  std::list<Result> wyniki=pca.recognise(midPt);
// 		  cout<<"finished in "<<time.elapsed()<<"s"<<endl;
// 		  for(std::list<Result>::iterator sit=wyniki.begin();
// 			   sit!=wyniki.end();++sit){
// 			 cout<<galleries.getGalleryLabel(sit->label)<<" "<<sit->mean
// 			     <<" "<<sit->max<<" "<<sit->min<<endl;
// 			 if(sit->mean<minValue){
// 			   minValue=sit->mean;
// 			   minLabel=galleries.getGalleryLabel(sit->label);
// 			 }
// 		  }
	   
// 		  putText(gemben,"PCA: "+minLabel,
// 			  Point(it->x,it->y+it->height+16),
// 			  FONT_HERSHEY_SIMPLEX,
// 			  1,Scalar(0,255,0));
		  
// 		  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 		      <<fmod(time.elapsed(),60) <<"s"<<endl;
// 		}
// #endif
// #ifdef PITTPATT_PRESENT
// 		{
// 		  string bestMatch;
// 		  cout<<"PPR recognising"<<endl;
// 		  time.restart();
// 		  std::list<Result> wyniki=pp.recognise(midPt);
// 		  cout<<"PPR recognised "<<endl;
// 		  for(std::list<Result>::iterator sit=wyniki.begin();
// 		      sit!=wyniki.end();++sit){
// 		    cout<<galleries.getGalleryLabel(sit->label)<<" "<<sit->mean
// 			<<" "<<sit->max<<" "<<sit->min<<endl;
// 		  }
// 		  bestMatch=wyniki.front().label;
		    
      
// 		  putText(gemben,"PPR: "+bestMatch,
// 			  Point(it->x,it->y+it->height+32),
// 			  FONT_HERSHEY_SIMPLEX,
// 			  1,Scalar(255,0,0));
		  
		  
// 		  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 		      <<fmod(time.elapsed(),60) <<"s"<<endl;
// 		}
// #endif
