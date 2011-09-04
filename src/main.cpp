#include <iostream>
#include "thread.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#include <vector>
#include <cmath>
#include <string>

#include <sstream> //do nazw plików

#include "Lapacz.hpp"

using namespace cv;
using namespace std;

const float FACE_FACTOR=0.2;


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
  Mat ggemben;
  
  string adres="galerie/test";


  long long counter=1;

  int m,n;

  vector<Rect> twarze; 
  
  Lapacz kam(0);

  CascadeClassifier szukacz;
  //  gpu::CascadeClassifier_GPU gszuk;

  
  cout<<gpu::getCudaEnabledDeviceCount()<<endl;
  
  szukacz.load(argv[1]);
  // try{
  //   gszuk.load(argv[1]);
  // }
  // catch(Exception ex){
  //   cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
  // }
  
  //CreateButton(nameb1,callbackButton,nameb1,CV_CHECKBOX,1);


  cout<<"hi thar"<<endl;
  namedWindow("in",CV_WINDOW_NORMAL);
  namedWindow("proces",CV_WINDOW_NORMAL);
  namedWindow("test",CV_WINDOW_NORMAL);
  //  namedWindow("gpu",CV_WINDOW_NORMAL);
  while(ster!='q'){
    try{
      kam.stopKlatka(obr);
      obr.copyTo(gemben);
      rozm=obr.size();
      cvtColor(obr,bw,CV_RGB2GRAY);
      equalizeHist(bw,eq[3]);
      cvtColor(eq[3],outDft,CV_GRAY2RGB);
      merge(eq,3,postEq);
      imshow("in",obr);
    }

    catch(Exception ex){
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
    }
    
    try{
        
      {
	stringstream sBufor;
	string cel;
	szukacz.detectMultiScale(eq[3],twarze,1.3);
	if(!twarze.empty()){
	  m=floor(sqrt(twarze.size()));
	  n=ceil(sqrt(twarze.size()));
	  mid.create(rozm.width*m*(1+FACE_FACTOR),rozm.width*n,CV_8UC3);
	  int i=0;
	  for(vector<Rect>::iterator it=twarze.begin();it!=twarze.end();++it,++i){
	    it->y-=(it->height)*FACE_FACTOR/2;
	    it->height*=(1+FACE_FACTOR);
	    rectangle(gemben,
		      Point(it->x,it->y),
		      Point(it->x+it->width,it->y+it->height),
		      Scalar(255,0,0));

	    Mat midPt=mid(Rect(rozm.width*(i/n),
			       rozm.width*(i%m)*(1+FACE_FACTOR),
			       rozm.width,rozm.width*(1+FACE_FACTOR)));
	    resize(Mat(obr,(*it)),midPt,midPt.size(),0,0,CV_INTER_LINEAR);
	    sBufor<<adres<<'/'<<counter++<<".jpg";
	    sBufor>>cel;
	    cerr<<cel<<endl;

	    Mat toWrite(rozm.width,rozm.height,CV_8U);
	    resize(Mat(eq[3],(*it)),toWrite,toWrite.size(),0,0,CV_INTER_LINEAR);

	    imwrite(cel,toWrite);
	    //  cerr<<twarze.size()<<" "<<m<<" "<<n<<endl;
	    //  cerr<<rozm.width*(i/n)<<endl;

	    //  cerr<<rozm.height*(i%m)<<endl;

	    //  cerr<<endl;
	  }
	}
      }

      // {
      // Mat image_cpu;
      // image_cpu=obr.clone();

      // gpu::GpuMat gmat(eq[3]);
      // gpu::GpuMat objbuf;

      // int detections_number = gszuk.detectMultiScale( gmat,
      // 						      objbuf, 1.2);
      
      // Mat obj_host;
      // // download only detected number of rectangles
      // objbuf.colRange(0, detections_number).download(obj_host);
      
      // Rect* faces = obj_host.ptr<Rect>();
      // for(int i = 0; i < detections_number; ++i)
      // 	cv::rectangle(image_cpu, faces[i], Scalar(255));
      // }

      //imshow("gpu",image_cpu);
      imshow("test",gemben);
      imshow("proces",mid);
      ster=waitKey(100);
    
    }
    catch(Exception ex){
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
    }
    
  }
  
  return 0;
}
