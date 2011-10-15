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


  /*
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
  */

  // pca.loadPrecomputedGalleries("PCA.xml");

  return 0;
}
