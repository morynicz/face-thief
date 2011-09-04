#include <iostream>
#include "thread.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#include <vector>
#include <list>
#include <cmath>
#include <string>


#include <sstream> //do nazw plików

#include "Lapacz.hpp"

using namespace cv;
using namespace std;

const float FACE_FACTOR=0.2;

struct Gallery{
  string label;
  long counter;
  list<string> photos;
};
    

int main(int argc,char **argv){

  Size rozm;
  Mat obr,postEq;
  Mat bgr[3],eq[4];
  char ster='1';
  Mat mid;
  Mat zera;

  Mat inDft,outDft;
  Mat czer;
  Mat bw;
  Mat gemben;
  Mat ggemben;
  
  string adres;


  long long counter=1;

  int m,n;

  vector<Rect> twarze; 
  
  Lapacz kam(0);

  CascadeClassifier szukacz;
  
  cout<<gpu::getCudaEnabledDeviceCount()<<endl;
  
  szukacz.load(argv[1]);
  adres=argv[2];


  /*
    FileStorage fs(string(adres+"/galeria.xml"),
		   FileStorage::READ);
    if(!fs.isOpened()){
      cv::Exception err(1,"file cannot be opened",
			__func__,__FILE__,__LINE__);
      throw err;
      }*/
    // //=========================================================
    //    tu powinno byc wczytanie do wektora galerii 
    //   poszczegolnych galerii z fs'a


  //    fs<<"image_list"<<"[";
  

  cout<<"hi thar"<<endl;
  namedWindow("in",CV_WINDOW_NORMAL);
  namedWindow("proces",CV_WINDOW_NORMAL);
  namedWindow("test",CV_WINDOW_NORMAL);
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
	
	//	fs<<"test"<<"[";


	stringstream sBufor;
	string cel,imie_nazwisko,buff;
	szukacz.detectMultiScale(eq[3],twarze,1.3);
	if(!twarze.empty()){
	  m=floor(sqrt(twarze.size()));
	  n=ceil(sqrt(twarze.size()));
	  mid.create(rozm.width*m*(1+FACE_FACTOR),rozm.width*n,CV_8UC3);
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
	    resize(Mat(obr,(*it)),midPt,midPt.size(),0,0,CV_INTER_LINEAR);
	    cout<<"kto to?"<<endl;
	    cin>>imie_nazwisko;
	    if(imie_nazwisko=="koniec")
	      break;
	    sBufor<<adres<<'/'<<imie_nazwisko<<'/'<<counter++<<".jpg";
	    sBufor>>cel;
	    cerr<<cel<<endl;
	    
	    Mat toWrite(rozm.width,rozm.height,CV_8U);
	    resize(Mat(eq[3],(*it)),toWrite,toWrite.size(),0,0,CV_INTER_LINEAR);

	    imwrite(cel,toWrite);
	    
	    //	    fs<<cel;
	  }
	  if(imie_nazwisko=="koniec")
	    break;
	}
	//	fs<<"]";
      }
  
      imshow("test",gemben);
      imshow("proces",mid);
      ster=waitKey(100);
    
    }
    catch(Exception ex){
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
    }
    
  }
  //===================================================
  // a tu zapis wektora galerii do fs'a

  //  fs<<"]";
  //  fs.release();
  return 0;
}
