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



void timeElapsed(const boost::timer &time){
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
      <<fmod(time.elapsed(),60) <<"s"<<endl;
}

int main(int argc,char **argv){


  Mat eq;
  Mat mid;
  Mat bw;
  Mat faces;

  string picture;

  Size rozm(OUT_WIDTH,OUT_HEIGHT);
  
  string adres;

  Galleries galleries;


  vector<Rect> foundFaces; 
  vector<Rec*> alg;


  CascadeClassifier finder;
   
  finder.load("kaskady/haarcascade_frontalface_alt_tree.xml");
  if(argc<3){
    cerr<<"Error: incorrect number of arguments. Correct invocation:"<<endl
	<<argv[0]<<" galleries_folder [-c capture_device_nr|-v video_file.avi]"
	<<endl;
    return 1;
  }
  string option=argv[2];
  Catcher cam;
  VideoCapture vid;
  int vc;

 
  adres=argv[1];

  // loading galleries
  try{
    galleries.setPath(adres);
    galleries.load("gallery.xml");
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
	cam.init(devNr);
	vc=-1;
      }else if(option=="-v"){
	
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
    {//Controll loop
      Mat img;
      int m,n;
      char controll='u';
      namedWindow("scanned",CV_WINDOW_NORMAL);
      namedWindow("in",CV_WINDOW_NORMAL);
      namedWindow("face",CV_WINDOW_NORMAL);
      while(controll!='q'){
       
	try{
	  if(-1==vc){
	  cam.catchFrame(img);
	  }else if(1==vc){
	    vid>>img;
	  }else{
	    return 1;
	  }
	  if(img.empty()){
	    return 0;
	  }
	  img.copyTo(faces);
	  cvtColor(img,bw,CV_RGB2GRAY);
	  equalizeHist(bw,eq);
	  imshow("in",img);
	}
	catch(Exception ex){
	  cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	      <<" in function "<<__func__<<endl;
	  throw ex;
	}   
	try{
	  finder.detectMultiScale(eq,foundFaces,1.3);
	  if(!foundFaces.empty()){
	    m=floor(sqrt(foundFaces.size()));
	    n=ceil(sqrt(foundFaces.size()));
	    mid.create(rozm.width*m*(1+FACE_FACTOR),rozm.width*n,eq.type());
	    int i=0;
	 
	    for(vector<Rect>::iterator it=foundFaces.begin();
		it!=foundFaces.end();++it,++i){
	      it->y-=(it->height)*FACE_FACTOR/2;
	      it->height*=(1+FACE_FACTOR);
	      rectangle(faces,
			Point(it->x,it->y),
			Point(it->x+it->width,it->y+it->height),
			Scalar(255,0,0));

		Mat midPt=mid(Rect(rozm.width*(i/n),
				   rozm.width*(i%m)*(1+FACE_FACTOR),
				   rozm.width,rozm.width*(1+FACE_FACTOR)));
		resize(Mat(eq,(*it)),midPt,midPt.size(),0,0,CV_INTER_LINEAR);
		imshow("face",midPt);

		{
		  Mat temp=midPt.clone();
		  string bestMatch;
		  for(int z=0;z<alg.size();++z){
		    cout<<alg[z]->getName()<<" recognising"<<endl;
		    time.restart();
		    std::list<Result> results=alg[z]->recognise(temp);
		    cout<<alg[z]->getName()<<" recognised "<<endl;
		    for(std::list<Result>::iterator sit=results.begin();
			sit!=results.end();++sit){
		      cout<<galleries.getGalleryLabel(sit->label)<<" "
			  <<sit->score<<endl;
		    }
		    bestMatch=galleries.getGalleryLabel(results.front().label);
		    cerr<<endl<<alg[z]->getName()+" "+bestMatch
			<<endl<<endl;
		    
		    putText(faces,alg[z]->getName()+" "+bestMatch,
			    Point(it->x,it->y+it->height+(z*2+2)*15),
			    FONT_HERSHEY_SIMPLEX,
			    1,Scalar(0,255,0),2);
		    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
			<<fmod(time.elapsed(),60) <<"s"<<endl;
		  }
		}
		imshow("scanned",faces);
		
	    }
	  }
	}
	catch(Exception ex){
	  cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	      <<" in function "<<__func__;
	  cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
	}
      
     	controll=waitKey(100);
	
      }
    }
  }
  return 0;
}
    
		
		
		
