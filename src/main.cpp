#include <iostream>
#include "thread.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#include <vector>
#include <cmath>

#include "Lapacz.hpp"

using namespace cv;
using namespace std;




int main(int argc,char **argv){
  //VideoCapture kam(0);
  Size rozm;
  Mat obr,postEq;
  Mat bgr[3],eq[4];
  char ster='1';
  Mat mid;
  Mat zera;
  //Mat inDft[2],outDft[2];
  Mat inDft,outDft;
  Mat czer;
  Mat bw;
  Mat gemben;

  int m,n;

  vector<Rect> twarze; 
  
  Lapacz kam(0);

  CascadeClassifier szukacz;
  //  gpu::CascadeClassifier_GPU gSzukacz;

  szukacz.load(argv[1]);
  //  gSzukacz.load(argv[1]);

  cout<<gpu::getCudaEnabledDeviceCount()<<endl;

  cout<<"hi thar"<<endl;
  namedWindow("in",CV_WINDOW_NORMAL);
  namedWindow("proces",CV_WINDOW_NORMAL);
  namedWindow("test",CV_WINDOW_NORMAL);
  namedWindow("post",CV_WINDOW_NORMAL);
  while(ster!='q'){
    try{
      kam.stopKlatka(obr);;
      obr.copyTo(gemben);
      rozm=obr.size();
      /*    zera=Mat::zeros(rozm,CV_8U);
	    split(obr,bgr);
	    mid.create(rozm.height*2,rozm.width*4,CV_8U);
	    for(int i=0;i<3;++i){
	    Mat midPt=mid(Rect(rozm.width*i,0,rozm.width,rozm.height));
	    resize(bgr[i],midPt,midPt.size(),0,0,CV_INTER_LINEAR);
	    }
	    for(int i=0;i<3;++i){
	    equalizeHist(bgr[i],eq[i]);
	    Mat midPt=mid(Rect(rozm.width*i,rozm.height,rozm.width,rozm.height));
	    resize(eq[i],midPt,midPt.size(),0,0,CV_INTER_LINEAR);
	    }
    
	    // inDft[0]=obr;
	    // inDft[1]=zera;
	    */
      cvtColor(obr,bw,CV_RGB2GRAY);
      equalizeHist(bw,eq[3]);
      //Mat midPt=mid(Rect(rozm.width*3,rozm.height,rozm.width,rozm.height));
      // resize(eq[3],midPt,midPt.size(),0,0,CV_INTER_LINEAR);
      cvtColor(eq[3],outDft,CV_GRAY2RGB);
      //    outDft.create(getOptimalDFTSize(bw.cols),getOptimalDFTSize(czer.rows),CV_8U);

      //    dft(czer,outDft,0,czer.rows);

      merge(eq,3,postEq);
      imshow("in",obr);
    }

    catch(Exception ex){
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
    }

   
    //    imshow("proces",mid);
    //  imshow("post",postEq);
    // imshow("test",outDft);

    try{
      //      gpu::gpuMat gObr(eq[3]);
      
      szukacz.detectMultiScale(eq[3],twarze,1.3);
      if(!twarze.empty()){
	m=floor(sqrt(twarze.size()));
	n=ceil(sqrt(twarze.size()));
	mid.create(rozm.width*m,rozm.width*n,CV_8UC3);
	int i=0;
	for(vector<Rect>::iterator it=twarze.begin();it!=twarze.end();++it,++i){
	  rectangle(gemben,
		    Point(it->x,it->y),
		    Point(it->x+it->width,it->y+it->height),
		    Scalar(255,0,0));

	  Mat midPt=mid(Rect(rozm.width*(i/n),
			     rozm.width*(i%m),
			     rozm.width,rozm.width));
	  resize(Mat(obr,(*it)),midPt,midPt.size(),0,0,CV_INTER_LINEAR);
	  /* imshow("post",midPt);
	     imshow("proces",mid);*/
	  //q waitKey(500);
	  cerr<<twarze.size()<<" "<<m<<" "<<n<<endl;
	  cerr<<rozm.width*(i/n)<<endl;
	  // cerr<<ceil(sqrt(twarze.size()))<<endl;
	  cerr<<rozm.height*(i%m)<<endl;
	  // cerr<<(int)(floor(sqrt(twarze.size())))<<endl;
	  cerr<<endl;
	  /* 
	     waitKey(500);*/
	}
      }
    
    imshow("test",gemben);
    imshow("proces",mid);
    ster=waitKey(1000);
    
    }
    catch(Exception ex){
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
    }

  }
  
return 0;
}
