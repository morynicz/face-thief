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
#include "PCARec.hpp"
#include "SVMRec.hpp"
#include "timer.hpp"

#include "PittpattPresence.hpp"
#ifdef PITTPATT_PRESENT
#include "PPRec.hpp"
#endif
#include <sstream> //do nazw plików

#include "Lapacz.hpp"

using namespace cv;
using namespace std;

const float FACE_FACTOR=0.0;


    

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
  
  //  Lapacz kam(0);

  //  CascadeClassifier szukacz;
  
  // szukacz.load(argv[1]);
  if(argc!=3){
    cerr<<"Error: incorrect number of arguments. Correct invocation:"<<endl
	<<argv[0]<<" galleries_folder photo_for_recognition"<<endl;
    return 1;
  }
  adres=argv[1];
  zdjecie=argv[2];

  Mat do_golenia=imread(zdjecie);
  Mat kompresowany;
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
   Mat pomiar=imread(zdjecie);


  boost::timer time;
  {
        
#ifdef PITTPATT_PRESENT

    try{
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
      {
	cout<<"PPR recognising"<<endl;
	time.restart();
	std::list<Result> wyniki=pp.recognise(pomiar);
	cout<<"PPR recognised "<<endl;
	for(std::list<Result>::iterator it=wyniki.begin();it!=wyniki.end();++it){
	  cout<<galleries.getGalleryLabel(it->label)<<" "<<it->mean
	      <<" "<<it->max<<" "<<it->min<<endl;
	}
      }
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	  <<fmod(time.elapsed(),60) <<"s"<<endl;
    }
    catch(Exception ex){
      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	  <<" in function "<<__func__<<endl;
      throw ex;
    }
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
	   
      putText(pomiar,"PCA: "+minLabel,
	      Point(it->x,it->y+it->height+16),
	      FONT_HERSHEY_SIMPLEX,
	      1,Scalar(0,255,0));
		  
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	  <<fmod(time.elapsed(),60) <<"s"<<endl;
    }
#endif
#ifdef PITTPATT_PRESENT
    {
      cout<<"PPR recognising"<<endl;
      time.restart();
      std::list<Result> wyniki=pp.recognise(pomiar);
      cout<<"PPR recognised "<<endl;
      for(std::list<Result>::iterator sit=wyniki.begin();
	  it!=wyniki.end();++it){
	cout<<galleries.getGalleryLabel(sit->label)<<" "<<sit->mean
	    <<" "<<sit->max<<" "<<sit->min<<endl;
      }
		  
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	  <<fmod(time.elapsed(),60) <<"s"<<endl;
    }
#endif
  }

  return 0;
}
  


  
  /*
    #ifdef PITTPATT_PRESENT

   try{
     cerr<<"PPR initialisng"<<endl;
     PPRec pp;
     // cerr<<"PPR initialised"<<endl;
     // cerr<<"PPR galleries loading"<<endl;
     // pp.loadGalleries(galleries);
     // cerr<<"PPR galleries loaded"<<endl;
     // cerr<<"PPR computing"<<endl;
     // pp.compute();
     // cerr<<"PPR computed"<<endl;
     
     // pp.savePrecomputedGalleries("PPGallery.ppr");
     

     // {
     //   cerr<<"PPR recognising"<<endl;
     //   std::list<Result> wyniki=pp.recognise(pomiar);
     //   cerr<<"PPR recognised "<<endl;
     //   for(std::list<Result>::iterator it=wyniki.begin();it!=wyniki.end();++it){
     // 	 cout<<galleries.getGalleryLabel(it->label)<<" "<<it->mean
     // 	     <<" "<<it->max<<" "<<it->min<<endl;
     //   }
     // }
     pp.loadPrecomputedGalleries("PPGallery.ppr");
     {
       cerr<<"PPR recognising"<<endl;
       std::list<Result> wyniki=pp.recognise(pomiar);
       cerr<<"PPR recognised "<<endl;
       for(std::list<Result>::iterator it=wyniki.begin();it!=wyniki.end();++it){
	 cout<<galleries.getGalleryLabel(it->label)<<" "<<it->mean
	     <<" "<<it->max<<" "<<it->min<<endl;
       }
     }
   }
   catch(Exception ex){
     cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	 <<" in fucntion "<<__func__<<endl;
     throw ex;
   }   
#endif


  
  PCARec pca;
  pca.loadGalleries(galleries);
  pca.compute();
  
  {
    std::list<Result> wyniki=pca.recognise(pomiar);
    
    for(std::list<Result>::iterator it=wyniki.begin();it!=wyniki.end();++it){
    cout<<galleries.getGalleryLabel(it->label)<<" "<<it->mean
      <<" "<<it->max<<" "<<it->min<<endl;
    }
    }
  //  pca.savePrecomputedGalleries("PCA.xml");
  
   
   SVMRec svm;
   svm.loadGalleries(galleries);
   svm.compute();
   {
    std::list<Result> wyniki=svm.recognise(pomiar);
    
    for(std::list<Result>::iterator it=wyniki.begin();it!=wyniki.end();++it){
      cout<<galleries.getGalleryLabel(it->label)<<" "<<it->mean
	  <<" "<<it->max<<" "<<it->min<<endl;
    }
   }

   
   // pca.loadPrecomputedGalleries("PCA.xml");
   
   return 0;
}
  */
