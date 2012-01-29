///\file
///\brief Main file of program recognizing people from photos

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <list>
#include <cmath>
#include <string>
#include <sstream>
#include <boost/filesystem.hpp>
#include "Galleries.hpp"
#include "PCARec.hpp"
#include "SVMRec.hpp"
#include <boost/timer.hpp>
#include "FaceFactor.hpp"
#include "PittpattPresence.hpp"
#ifdef PITTPATT_PRESENT
#include "PPRec.hpp"

#define PPREC_PRECOMPUTED

#endif
#include <sstream> 

#define PCAREC
#define PCAREC_PRECOMPUTED
#define SVMREC
#define SVMREC_PRECOMPUTED

using namespace cv;
using namespace std;

/// Program demonstrating how the face recognition algorithms work on single 
/// picture(s).
int main(int argc,char **argv){


  Mat img;
  Mat eq;
  Mat mid;
  Mat bw;
  Mat faces;

  string photo;

  Size size(OUT_WIDTH,OUT_HEIGHT);
  
  string address;

  Galleries galleries;


  vector<Rect> foundFaces; 
  vector<Rec*> alg;


  //  CascadeClassifier finder;
  
 
  if(argc<2){
    cerr<<"Error: incorrect number of arguments. Correct invocation:"<<endl
	<<argv[0]<<" galleries_folder [photo_for_recognition]"<<endl;
    return 1;
  }
  address=argv[1];

  CascadeClassifier finder("kaskady/haarcascade_frontalface_alt_tree.xml");

  if(argc==3){
    photo=argv[2];
  }
  try{
    galleries.setPath(address);
    galleries.load("gallery.xml");
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in fucntion "<<__func__;
    cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
  }
  
  cout<<"hi thar"<<endl;

   boost::timer time;
   
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
#endif

    {//Control loop
      Mat img;
      int m,n;
      char controll;
      if(argc==3)
	controll='\000';
      else
	controll='u';

      namedWindow("scanned",CV_WINDOW_NORMAL|CV_WINDOW_KEEPRATIO);
      namedWindow("in",CV_WINDOW_NORMAL);
      namedWindow("found",CV_WINDOW_NORMAL);
      long counter=0;
    while(controll!='n'){
       
	try{
	  if(controll!='r'&&controll!='\000'){
	    cout<<"please name the photo to be read: "<<endl;
	    cin>>photo;
	  }
	  img=imread(photo);
	  if(img.empty()){
	    cerr<<"could not read the image!"<<endl;
	    continue;
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
	    mid.create(size.height*m,size.width*n,eq.type());
	    int i=0;
	 
	    for(vector<Rect>::iterator it=foundFaces.begin();
		it!=foundFaces.end();++it,++i){
	      it->y-=(it->height)*FACE_FACTOR/2;
	      it->height*=(1+FACE_FACTOR);
	      rectangle(faces,
			Point(it->x,it->y),
			Point(it->x+it->width,it->y+it->height),
			Scalar(255,0,0));

	      Mat midPt=mid(Rect(size.width*(i/n),
				 size.height*(i%m),
				 size.width,size.height));
	      resize(Mat(eq,(*it)),midPt,midPt.size(),0,0,CV_INTER_LINEAR);
	  
	      {
		string bestMatch;
		for(int z=0;z<alg.size();++z){
		  try{
		    cout<<alg[z]->getName()<<" recognising"<<endl;
		    time.restart();
		    Mat tmp=eq.clone();
		    std::list<Result> results=alg[z]->recognise(tmp);
		    cout<<alg[z]->getName()<<" recognised "<<endl;
		    for(std::list<Result>::iterator sit=results.begin();
			sit!=results.end();++sit){
		      cout<<galleries.getGalleryLabel(sit->label)<<" "
			  <<sit->score<<endl;
		    }
		   
		    if(!results.empty()){
		      bestMatch=galleries.getGalleryLabel(results.front().label);
		      cerr<<endl<<alg[z]->getName()+" "+bestMatch
			  <<endl<<endl;
		    }		    
		    putText(faces,alg[z]->getName()+" "+bestMatch,
		    	    Point(it->x,it->y+it->height+(z*2.5+2)*10),
		    	    FONT_HERSHEY_SIMPLEX,
		    	    1,Scalar(0,255,0),2);
		    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
			<<fmod(time.elapsed(),60) <<"s"<<endl;
		  }
		  catch(Exception ex){
		    cerr<<ex.code<<" "<<ex.err<<endl
			<<ex.func<<endl<<ex.line<<endl;
		  }
		
		}
		}
	      imshow("scanned",faces);
	      imshow("found",mid);
	    }
	    {
		string name;
		std::stringstream buff;
		buff<<"out"<<counter++<<".jpg";
		buff>>name;
		imwrite(name,faces);
		}
	    }
	}
	catch(Exception ex){
	  cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	      <<" in function "<<__func__;
	  cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
	}
	
     	waitKey(100);
	if(controll!='\000'){
	  cout<<"Wczytać kolejne zdjęcie (n-nie/r-powtórz to \
samo/cokolwiek-tak): "<<endl;
	  cout<<"Read another image? (n - no / r - repeat on the previous one\
any_other_letter - yes"<<endl;
	  cin>>controll;
	}else{
	  break;
	}
      }
    }
    return 0;
}
