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
#if HOSTNAME==rab
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
  // namedWindow("in",CV_WINDOW_NORMAL);
  // namedWindow("proces",CV_WINDOW_NORMAL);
  // namedWindow("test",CV_WINDOW_NORMAL);
  // namedWindow("gemba",CV_WINDOW_NORMAL);
  // namedWindow("z_galerii",CV_WINDOW_NORMAL);

   Mat pomiar=imread(zdjecie);

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

#if HOSTNAME==rab

   try{
  PPRec pp;
  pp.loadGalleries(galleries);
  pp.compute();
  
  {
    std::list<Result> wyniki=pp.recognise(pomiar);

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
  // pca.loadPrecomputedGalleries("PCA.xml");
 
  // {
  //   std::list<Result> wyniki=pca.recognise(pomiar);
  //   cerr<<wyniki.size()<<endl;
  //   for(std::list<Result>::iterator it=wyniki.begin();it!=wyniki.end();++it){
  //     cout<<galleries.getGalleryLabel(it->label)<<" "<<it->mean
  // 	  <<" "<<it->max<<" "<<it->min<<endl;
  //   }
  // } 
      
  /*
  {//PCA
    { //Odczyt i przejście na tablice wektorow
      int rows=0;
      int cols=galleries.getPictureSize().width
	*galleries.getPictureSize().height;
      for(int i=0;i<galleries.totalSize();++i){
	rows+=galleries.gallerySize(i);
      }
      
      Mat input(rows,cols,CV_8U);
      
      int y=0;
      for(int i=0;i<galleries.totalSize();++i){
	for(int j=0;j<galleries.gallerySize(i);++j){
	  Mat img=galleries.getPicture(i,j);
	  Mat bw; //needed or OCV2.2 would segment fault

	  if(img.channels()!=1){
	    Mat tmp;
	    cvtColor(img,tmp,CV_RGB2GRAY);
	    equalizeHist(tmp,bw);
	  }else{
	    bw=img;
	  }
	  
	  Mat reshaped=bw.reshape(1,1);
	  Mat inInput=input.row(y++);
	  resize(reshaped,inInput,inInput.size(),0,0,CV_INTER_LINEAR);
	
	}
      }
      
    
      int maxComponents=cols*0;
      cerr<<maxComponents<<endl;
      PCA pca(input,Mat(),CV_PCA_DATA_AS_ROW,maxComponents);
   
      //      Mat compressed(rows,maxComponents,CV_8U);
      Mat compressed;
      pca.project(input,compressed);


      

	 
	 cerr<<do_golenia.cols<<do_golenia.rows<<do_golenia.channels()<<endl;

	 Mat ch,ch2;
	 cvtColor(do_golenia,ch2,CV_RGB2GRAY);
	 
      cerr<<ch2.type()<<endl;
      cerr<<ch2.cols<<" "<<ch2.rows<<endl;
      cerr<<input.cols<<endl;
      pca.project(ch2.reshape(1,1),kompresowany);



      cerr<<compressed.cols<<" "<<compressed.rows<<endl;
      cerr<<compressed.depth()<<compressed.channels()<<endl;
      cerr<<pca.eigenvectors.cols<<" "<<pca.eigenvectors.rows<<endl;
      Mat reconstructed;

      pca.backProject(compressed,reconstructed);

      // 
      // for(int i=0;i<rows;++i){
      // 	Mat img=reconstructed.row(i).reshape(0,galleries.getPictureSize().width);
      // 	Mat umg=input.row(i).reshape(0,galleries.getPictureSize().width);
      // 	Mat emg=pca.eigenvectors.row(i).reshape(0,galleries.getPictureSize().width);
	
      // 	imshow("in",img);
      // 	imshow("proces",umg);
      // 	imshow("test",emg*100);
      // 	waitKey(5100);

      // }
      // 

      Mat covar;
      Mat mean;
      Mat icovar;
      
      calcCovarMatrix(compressed,covar,mean,
		      CV_COVAR_NORMAL|CV_COVAR_ROWS,
		      compressed.type());
      invert(covar,icovar,DECOMP_SVD);

      for(int i=0;i<rows;++i){
	 double dist2=Mahalanobis(compressed.row(i),kompresowany,icovar.t());
	 cerr<<i<<" "<<dist2<<endl<<endl;
	for(int j=0;j<rows;++j){
	  Mat in[2];//coord i , j
	 
	  in[0]=compressed.row(i);
	  in[1]=compressed.row(j);
	 
	  double dist=Mahalanobis(in[0],in[1],icovar);
	 
	  cerr<<i<<" "<<j<<" "<<dist<<endl;
	
	
	}
      }
      
    }
  }
  */


  return 0;
}
