#include <iostream>
#include "thread.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#include <vector>
#include <list>
#include <cmath>
#include <string>
#include "filesystem.hpp"
#include "timer.hpp"
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
   {
     boost::timer time;


     PCARec pca2;

     cout<<"PCA2: Loading precomputed galleries"<<endl;
     time.restart();
     pca2.loadPrecomputedGalleries("PCAdata/PCA.xml");
     cout<<"finished in "<<time.elapsed()<<"s"<<endl;


     
     cout<<"Starting PCA"<<endl;
     PCARec pca;
     cout<<"finished in "<<time.elapsed()<<"s"<<endl;
     cout<<"PCA: Loading galleries"<<endl;
     pca.loadGalleries(galleries);
     cout<<"finished in "<<time.elapsed()<<"s"<<endl;
     
     cout<<"PCA: Computing"<<endl;
     time.restart();
     pca.compute();
     cout<<"finished in "<<time.elapsed()<<"s"<<endl;



     cerr<<(pca2._pca.mean.rows == pca._pca.mean.rows)<<endl;
     cerr<<(pca2._pca.mean.cols == pca._pca.mean.cols)<<endl;
     cerr<<(pca2._pca.mean.depth() == pca._pca.mean.depth())<<endl;
     cerr<<(pca2._pca.mean.channels() == pca._pca.mean.channels())<<endl;
     
     cerr<<sum(pca2._pca.eigenvectors == pca._pca.eigenvectors)[0]<<endl;
     cerr<<sum(pca2._pca.eigenvalues == pca._pca.eigenvalues)[0]<<endl;
     cerr<<sum(pca2._pca.mean == pca._pca.mean)[0]<<endl;
     cerr<<sum(pca2._vectors==pca._vectors)[0]<<endl;
     // {
     //   namedWindow("m1",CV_WINDOW_NORMAL);
     //   namedWindow("m2",CV_WINDOW_NORMAL);
     //   namedWindow("m3",CV_WINDOW_NORMAL);
     //   Mat m3;
       
     //   absdiff(pca._pca.mean,pca2._pca.mean,m3);
     //   imshow("m3",m3.reshape(1,260));
     //   imshow("m2",pca2._pca.mean.reshape(1,260));
     //   imshow("m1",pca._pca.mean.reshape(1,260));
     //   waitKey(50000);
     // }


     cerr<<"PCA: Computing"<<endl;
     time.restart();
     pca.compute();
     cerr<<"finished in "<<time.elapsed()<<"s"<<endl;
     cout<<"PCA: recognising"<<endl;
     
     {
       time.restart();
       std::list<Result> wyniki=pca.recognise(pomiar);
       cout<<"finished in "<<time.elapsed()<<"s"<<endl;
       for(std::list<Result>::iterator it=wyniki.begin();it!=wyniki.end();++it){
     	 cout<<galleries.getGalleryLabel(it->label)<<" "<<it->mean
     	     <<" "<<it->max<<" "<<it->min<<endl;
       }
     }
     cout<<"finished in "<<time.elapsed()<<"s"<<endl;
     cout<<"PCA: saving"<<endl;
     time.restart();
     pca.savePrecomputedGalleries("PCAdata/PCA.xml");
     cout<<"finished in "<<time.elapsed()<<"s"<<endl;
   

   
   //   cout<<"PCA2: recognising"<<endl;
   
   //   {
   //     time.restart();
   //     std::list<Result> wyniki=pca2.recognise(pomiar);
   //     cout<<"finished in "<<time.elapsed()<<"s"<<endl;
   //     for(std::list<Result>::iterator it=wyniki.begin();it!=wyniki.end();++it){
   //   	 cout<<galleries.getGalleryLabel(it->label)<<" "<<it->mean
   //   	     <<" "<<it->max<<" "<<it->min<<endl;
   //     }
   //   }
   }

 /*
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
