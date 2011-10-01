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

const float FACE_FACTOR=0.0;


    

int main(int argc,char **argv){


  Mat obr,eq;
  char ster='1';
  Mat mid;
  //  Mat zera;

  //  Mat inDft,outDft;
  //  Mat czer;
  Mat bw;
  Mat gemben;

  int outWidth=200;
  int outHeight=outWidth*(1+FACE_FACTOR);


  Size rozm(outWidth,outHeight);
  
  string adres;

  //  list<Gallery> galeries; 
  Galleries galleries;
  long long counter=1;

  int m,n;
  int numerZdjecia,numerGalerii;

  vector<Rect> twarze; 
  vector<Rect> lOka;
  vector<Rect> pOka;

  Lapacz kam(0);

  CascadeClassifier szukacz;
  CascadeClassifier lewe;
  CascadeClassifier prawe;

  szukacz.load(argv[1]);
  lewe.load("kaskady/haarcascade_lefteye_2splits.xml");
  prawe.load("kaskady/haarcascade_righteye_2splits.xml");

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
	    cerr<<"puff"<<endl;
	    equalizeHist(tmp,bw);
	  }else{
	    bw=img;
	  }
	  Mat reshaped=bw.reshape(1,1);
	  Mat inInput=input.row(y++);
	  resize(reshaped,inInput,inInput.size(),0,0,CV_INTER_LINEAR);
	
	}
      }
      
    
      int maxComponents=cols*1;
      cerr<<maxComponents<<endl;
      PCA pca(input,Mat(),CV_PCA_DATA_AS_ROW,maxComponents);
   
      Mat compressed(rows,maxComponents,CV_8U);


      pca.project(input,compressed);
      
      Mat reconstructed;

      pca.backProject(compressed,reconstructed);


      for(int i=0;i<rows;++i){
	Mat img=reconstructed.row(i).reshape(0,galleries.getPictureSize().width);
	Mat umg=input.row(i).reshape(0,galleries.getPictureSize().width);
	imshow("in",img);
	imshow("proces",umg);
	waitKey(5000);

      }


      for(int i=0;i<rows;++i){
	for(int j=0;j<rows;++j){
	  Mat in[2];//coord i , j
	  Mat covar;
	  Mat mean;
	  Mat icovar;
	  in[0]=compressed.row(i);
	  in[1]=compressed.row(j);
	  
	  calcCovarMatrix(in,2,covar,mean,CV_COVAR_NORMAL);

	  invert(covar,icovar,DECOMP_SVD);
	  //	  double dist=Mahalanobis(in[0],in[1],icovar.t());

	  //	calcCovarMatrix// covar mat
	  // double dist=mahalanobis(//coord i //coord j , covar^-1)
	}
      }
      
    }
  }
  */



  
   while(ster!='q'){
    try{
      kam.stopKlatka(obr);
      obr.copyTo(gemben);
      //rozm=obr.size();
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
	  mid.create(rozm.width*m*(1+FACE_FACTOR),rozm.width*n,eq.type());
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
	    resize(Mat(eq,(*it)),midPt,midPt.size(),0,0,CV_INTER_LINEAR);
	    
	    lewe.detectMultiScale(midPt,lOka,1.3);
	    for(int j=0;j<lOka.size();++j){
	      rectangle(midPt,Point(lOka[j].x,lOka[j].y),
			Point(lOka[j].x+lOka[j].width,lOka[j].y+lOka[j].height),
			Scalar(0,255,0));
	    }
	    prawe.detectMultiScale(midPt,pOka,1.3);
	    for(int j=0;j<pOka.size();++j){
	      rectangle(midPt,Point(pOka[j].x,pOka[j].y),
			Point(pOka[j].x+pOka[j].width,pOka[j].y+pOka[j].height),
			Scalar(0,0,255));
	    }
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
  
    //===========zapis
    
  galleries.save("galeria.xml");
  
  //koniec zapisu
  
  return 0;
}
