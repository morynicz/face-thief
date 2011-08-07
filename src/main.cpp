#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main(){
  VideoCapture kam(0);
  Size rozm;
  Mat obr,postEq;
  Mat bgr[3],eq[3];
  char ster='1';
  Mat mid;
  Mat zera;
  //Mat inDft[2],outDft[2];
  Mat inDft,outDft;
  Mat czer;
  
  cout<<"hi thar"<<endl;
  namedWindow("in",CV_WINDOW_NORMAL);
  namedWindow("proces",CV_WINDOW_NORMAL);
  namedWindow("test",CV_WINDOW_NORMAL);
  namedWindow("post",CV_WINDOW_NORMAL);
  while(ster!='q'){
    kam>>obr;
    rozm=obr.size();
    zera=Mat::zeros(rozm,CV_8U);
    split(obr,bgr);
    mid.create(rozm.height*2,rozm.width*3,CV_8U);
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

    czer=bgr[2];

    outDft.create(getOptimalDFTSize(czer.cols),getOptimalDFTSize(czer.rows),CV_8U);

    dft(czer,outDft,0,czer.rows);

    merge(eq,3,postEq);


    imshow("in",obr);
    imshow("proces",mid);
    imshow("post",postEq);
    imshow("test",outDft);
    ster=waitKey(100);
    
  }
  
return 0;
}
