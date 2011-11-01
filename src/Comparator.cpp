#include <iostream>
#include "thread.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#include <vector>
#include <list>
#include <cmath>
#include <string>
#include <sstream>
#include "filesystem.hpp"
#include "Galleries.hpp"
#include "PCARec.hpp"
#include "SVMRec.hpp"
#include "timer.hpp"
#include "FaceFactor.hpp"
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
#define SVMREC_PRECOMPUTED

using namespace cv;
using namespace std;

//const float FACE_FACTOR=0.0;


    

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
  vector<Rec*> alg;
  //  Lapacz kam(0);

  CascadeClassifier szukacz;
  
  // szukacz.load(argv[1]);
 
  if(argc<2){
    cerr<<"Error: incorrect number of arguments. Correct invocation:"<<endl
	<<argv[0]<<" galleries_folder [photo_for_recognition]"<<endl;
    return 1;
  }
  adres=argv[1];
  szukacz.load("kaskady/haarcascade_frontalface_alt_tree.xml");
  if(argc==3){
    zdjecie=argv[2];
  }
  Mat do_golenia;
  Mat kompresowany;
  do_golenia=imread(zdjecie);
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
  //  Mat pomiar=imread(zdjecie);


   boost::timer time;

        
#ifdef PITTPATT_PRESENT
      cout<<"PPR initialisng"<<endl;
      time.restart();
      //      PPRec pp;
      alg.push_back(new PPRec);
      alg.back()->initialise();
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
      	  alg.back()->savePrecomputedGalleries("PPGallery.ppr");
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
      	  <<fmod(time.elapsed(),60) <<"s"<<endl;

#elseif
      cout<<"PPR loading precomputed galleries"<<endl;
      time.restart();
      alg.back()->loadPrecomputedGalleries("PPGallery.ppr");
      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	  <<fmod(time.elapsed(),60) <<"s"<<endl;
#endif   

#endif
 
#ifdef PCAREC
    cout<<"Starting PCA"<<endl;
    //PCARec pca;
    alg.push_back(new PCARec);
    alg.back()->initialise();
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
    alg.back()->savePrecomputedGalleries("PCAdata/PCA.xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
#else
    cout<<"PCA: Loading precomputed galleries"<<endl;
    time.restart();
    alg.back()->loadPrecomputedGalleries("PCAdata/PCA.xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;

#endif
#endif
   
#ifdef SVMREC
    cerr<<"SVM starting"<<endl;
    time.restart();
    //SVMRec svm;
    alg.push_back(new SVMRec);
    alg.back()->initialise();
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
    alg.back()->savePrecomputedGalleries("SVMdata/SVM.xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
#else    
    cout<<"SVM: Loading precomputed galleries"<<endl;
    time.restart();
    alg.back()->loadPrecomputedGalleries("SVMdata/SVM.xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
#endif
#endif

    {//Control loop
      Mat obr;
      int m,n;
      char control;
      if(argc==3)
	control='r';
      else
	control='u';

      namedWindow("skanowane",CV_WINDOW_NORMAL|CV_WINDOW_KEEPRATIO);
      namedWindow("in",CV_WINDOW_NORMAL);
      namedWindow("znalezione",CV_WINDOW_NORMAL);
      long counter=0;
    while(control!='n'){
       
	try{
	  if(control!='r'){
	    cout<<"podaj nazwe zdjęcia do wczytania: "<<endl;
	    cin>>zdjecie;
	  }
	  obr=imread(zdjecie);
	  if(obr.empty()){
	    cerr<<"nie ma takiego zdjęcia!"<<endl;
	    continue;
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
	      //	      imshow("gemba",midPt);

	      {
		string bestMatch;
		for(int z=0;z<alg.size();++z){
		  cout<<alg[z]->getName()<<" recognising"<<endl;
		  time.restart();
		  std::list<Result> wyniki=alg[z]->recognise(midPt);
		  cout<<alg[z]->getName()<<" recognised "<<endl;
		  for(std::list<Result>::iterator sit=wyniki.begin();
		      sit!=wyniki.end();++sit){
		    cout<<galleries.getGalleryLabel(sit->label)<<" "<<sit->mean
			<<" "<<sit->max<<" "<<sit->min<<endl;
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
	      imshow("znalezione",mid);
	    }
	    {
		string name;
		std::stringstream buff;
		buff<<"out"<<counter++<<".jpg";
		buff>>name;
		imwrite(name,gemben);
	    }
	  }
	}
	catch(Exception ex){
	  cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	      <<" in function "<<__func__;
	  cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
	}
	
     	waitKey(100);
	cout<<"Wczytać kolejne zdjęcie (n-nie/r-powtórz to samo/cokolwiek-tak): "
	    <<endl;
	cin>>control;
      }
    }
    return 0;
}



   //   try{
        
// #ifdef PITTPATT_PRESENT
    
//      cout<<"PPR initialisng"<<endl;
//      time.restart();
//      PPRec pp;
//      cout<<"PPR initialised"<<endl;
//      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	 <<fmod(time.elapsed(),60) <<"s"<<endl;
     

// #ifndef PPREC_PRECOMPUTED
//        cout<<"PPR galleries loading"<<endl;
//        time.restart();
//        pp.loadGalleries(galleries);
//        cout<<"PPR galleries loaded"<<endl;
//        cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	   <<fmod(time.elapsed(),60) <<"s"<<endl;

//        cout<<"PPR computing"<<endl;
//        time.restart();
//        pp.compute();
//        cout<<"PPR computed"<<endl;
      
//        cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
//       	   <<fmod(time.elapsed(),60) <<"s"<<endl;
//        time.restart();
//        cout<<"PPR saving precomputed galleries"<<endl;
//        pp.savePrecomputedGalleries("PPRdata/PPGallery.ppr");
//        cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	   <<fmod(time.elapsed(),60) <<"s"<<endl;

// #else
//        cout<<"PPR loading precomputed galleries"<<endl;
//        time.restart();
//        pp.loadPrecomputedGalleries("PPRdata/PPGallery.ppr");
//        cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	   <<fmod(time.elapsed(),60) <<"s"<<endl;
// #endif   
// #endif
 
// #ifdef PCAREC
//      cout<<"Starting PCA"<<endl;
//      PCARec pca;
// #ifndef PCAREC_PRECOMPUTED
//      cout<<"finished in "<<time.elapsed()<<"s"<<endl;
//      cout<<"PCA: Loading galleries"<<endl;
//      pca.loadGalleries(galleries);
//      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	 <<fmod(time.elapsed(),60) <<"s"<<endl;
     
//      cout<<"PCA: Computing"<<endl;
//      time.restart();
//      pca.compute();
//      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	 <<fmod(time.elapsed(),60) <<"s"<<endl;


//      // cout<<"PCA: saving"<<endl;
//      // time.restart();
//      // pca.savePrecomputedGalleries("PCAdata/PCA.xml");
//      // cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
//      // 	 <<fmod(time.elapsed(),60) <<"s"<<endl;
// #else
//      cout<<"PCA: Loading precomputed galleries"<<endl;
//      time.restart();
//      pca.loadPrecomputedGalleries("PCAdata/PCA.xml");
//      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	 <<fmod(time.elapsed(),60) <<"s"<<endl;

// #endif
// #endif
   
// #ifdef SVMREC
//      cerr<<"SVM starting"<<endl;
//      time.restart();
//      SVMRec svm;
//      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	 <<fmod(time.elapsed(),60) <<"s"<<endl;

// #ifndef SVMREC_PRECOMPUTED
//      cerr<<"SVM: loading galleries"<<endl;
//      time.restart();
//      svm.loadGalleries(galleries);
//      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	 <<fmod(time.elapsed(),60) <<"s"<<endl;
    
//      cerr<<"SVM: computing"<<endl;
//      time.restart();
//      svm.compute();
//      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	 <<fmod(time.elapsed(),60) <<"s"<<endl;

//      // cout<<"SVM: saving"<<endl;
//      // time.restart();
//      // svm.savePrecomputedGalleries("SVMdata/SVM.xml");
//      // cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
//      // 	 <<fmod(time.elapsed(),60) <<"s"<<endl;
// #else    
//      cout<<"SVM: Loading precomputed galleries"<<endl;
//      time.restart();
//      svm.loadPrecomputedGalleries("SVMdata/SVM.xml");
//      cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	 <<fmod(time.elapsed(),60) <<"s"<<endl;
// #endif
// #endif
  		
// #ifdef SVMREC
//      try{
//        string posLabel;
//        cerr<<"SVM: recognising"<<endl;
//        time.restart();
		  
		  
//        std::list<Result> wyniki=svm.recognise(pomiar);
//        cout<<"finished in "<<time.elapsed()<<"s"<<endl;
//        for(std::list<Result>::iterator sit=wyniki.begin();
// 	   sit!=wyniki.end();++sit){
// 	 cout<<galleries.getGalleryLabel(sit->label)<<" "<<sit->mean
// 	     <<" "<<sit->max<<" "<<sit->min<<endl;
// 	 if(sit->mean>0)
// 	   posLabel=posLabel+" "+galleries.getGalleryLabel(sit->label);
//        }		  

//        if(posLabel.size()==0){
// 	 posLabel="unknown";
//        }

//        cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	   <<fmod(time.elapsed(),60) <<"s"<<endl;

//      }
    
//      catch(Exception ex){
//        cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
// 	   <<" in function "<<__func__<<endl;
//        throw ex;
//      }
// #endif		
// #ifdef PCAREC
//      try{
//        cout<<"PCA: recognising"<<endl;
    
//        string minLabel="unknown";
//        double minValue=10;
	     
//        time.restart();
//        std::list<Result> wyniki=pca.recognise(pomiar);
//        cout<<"finished in "<<time.elapsed()<<"s"<<endl;
//        for(std::list<Result>::iterator sit=wyniki.begin();
// 	   sit!=wyniki.end();++sit){
// 	 cout<<galleries.getGalleryLabel(sit->label)<<" "<<sit->mean
// 	     <<" "<<sit->max<<" "<<sit->min<<endl;
// 	 if(sit->mean<minValue){
// 	   minValue=sit->mean;
// 	   minLabel=galleries.getGalleryLabel(sit->label);
// 	 }
//        }
	   
		  
//        cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	   <<fmod(time.elapsed(),60) <<"s"<<endl;
//      }
    
//      catch(Exception ex){
//        cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
// 	   <<" in function "<<__func__<<endl;
//        throw ex;
//      }
// #endif
// #ifdef PITTPATT_PRESENT
//      try{
//        cout<<"PPR recognising"<<endl;
//        time.restart();
//        std::list<Result> wyniki=pp.recognise(pomiar);
//        cout<<"PPR recognised "<<endl;
//        for(std::list<Result>::iterator sit=wyniki.begin();
// 	   sit!=wyniki.end();++sit){
// 	 cout<<galleries.getGalleryLabel(sit->label)<<" "<<sit->mean
// 	     <<" "<<sit->max<<" "<<sit->min<<endl;
//        }
		  
//        cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
// 	   <<fmod(time.elapsed(),60) <<"s"<<endl;
//      }
    
//      catch(Exception ex){
//        cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
// 	   <<" in function "<<__func__<<endl;
//        throw ex;
//      }
// #endif
//    }
//    catch(Exception ex){
//      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
// 	 <<" in fucntion "<<__func__<<endl;
//      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
//    }
 
//    return 0;
// }



  
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
