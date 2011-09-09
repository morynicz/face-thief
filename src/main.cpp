#include <iostream>
#include "thread.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#include <vector>
#include <list>
#include <cmath>
#include <string>
#include "filesystem.hpp"

#include <sstream> //do nazw plików

#include "Lapacz.hpp"

using namespace cv;
using namespace std;

const float FACE_FACTOR=0.2;
const string GALERIES="Galeries";
const string LABEL="Label";
const string COUNTER="COUNTER";
const string ADDRES="Addres";


struct Gallery{
  string label;
  int counter;
  vector<string> photos;
  vector<Mat> pictures;
};
    

int main(int argc,char **argv){

  Size rozm;
  Mat obr,eq;
  char ster='1';
  Mat mid;
  Mat zera;

  Mat inDft,outDft;
  Mat czer;
  Mat bw;
  Mat gemben;
  Mat ggemben;
  
  string adres;

  list<Gallery> galeries; 

  long long counter=1;

  int m,n;

  vector<Rect> twarze; 
  
  Lapacz kam(0);

  CascadeClassifier szukacz;
  
  szukacz.load(argv[1]);
  adres=argv[2];

  // wczytywanie galerii zdjęć

  try{

    FileStorage fs(string(adres+"/galeria.xml"),
		   FileStorage::READ);
    if(!fs.isOpened()){
      cv::Exception err(1,"file cannot be opened",
			__func__,__FILE__,__LINE__);
      throw err;
    }
    
    //    FileNode n=fs.getFirstTopLevelNode();
    FileNode gals=fs["Galeries"];
    FileNodeIterator it=gals.begin();
    
    for(;it!=gals.end();++it){
      Gallery gal;
      (*it)[LABEL]>>gal.label;
      (*it)[COUNTER]>>gal.counter;

#if CV_MINOR_VERSION > 3
      (*it)[ADDRES]>>gal.photos;
      cerr<<gal.label<<endl<<gal.counter<<endl;
      for(unsigned i=0;i<gal.photos.size();++i){
	Mat img;
	cerr<<gal.photos[i]<<endl;
	img=imread(gal.photos[i]);
	gal.pictures.push_back(img);
      }
#else
      FileNode gfn=(*it)[ADDRES];
      FileNodeIterator git=gfn.begin();
      for(;git!=gfn.end();++git){
	Mat img;
	gal.photos.push_back((string)(*git));
	cerr<<gal.photos.back()<<endl;
	img=imread(gal.photos.back());
	gal.pictures.push_back(img);
      }

#endif
      galeries.push_back(gal);
      
    }
   
    fs.release();
   
  } 

  // wczytywanie zakończone

  catch(Exception ex){
    cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
  }
  
  cout<<"hi thar"<<endl;
  namedWindow("in",CV_WINDOW_NORMAL);
  namedWindow("proces",CV_WINDOW_NORMAL);
  namedWindow("test",CV_WINDOW_NORMAL);
  namedWindow("gemba",CV_WINDOW_NORMAL);
  while(ster!='q'){
    try{
      kam.stopKlatka(obr);
      obr.copyTo(gemben);
      rozm=obr.size();
      cvtColor(obr,bw,CV_RGB2GRAY);
      equalizeHist(bw,eq);
      imshow("in",obr);
    }

    catch(Exception ex){
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
	  mid.create(rozm.width*m*(1+FACE_FACTOR),rozm.width*n,CV_8UC3);
	  int i=0;
	  
	  list<Gallery>::iterator git;
	  
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
	    
	  
	    
	    imshow("gemba",midPt);
	    if(ster!='q'){
	      ster=' ';
	      ster=waitKey(1000);
	      if(ster=='c'){
		cout<<"kto to?"<<endl;
		cin>>label;
		if(label=="koniec")
		  break;
		if(!galeries.empty()){


		  for(git=galeries.begin();
		      git!=galeries.end();
		      ++git){
		    cerr<<"szuka "<<label<<" == "<<git->label<<'?'<<endl;
		    if(git->label==label)
		      break;
		  }
	      
		  if(git==galeries.end()){
		    Gallery gal;
		    gal.label=label;
		    gal.counter=0;
		    --git;
		    galeries.push_back(gal);
		    ++git;
		    if(!boost::filesystem::exists(adres+'/'+label)){
		      boost::filesystem::create_directory(adres+'/'+label);
		      cerr<<(adres+'/'+label)<<endl;
		    }
		  }
		}else{
		  Gallery gal;
		  gal.label=label;
		  gal.counter=0;
		  galeries.push_back(gal);
		  git=galeries.begin();
		  if(!boost::filesystem::exists(adres+'/'+label)){
		    boost::filesystem::create_directory(adres+'/'+label);
		    cerr<<(adres+'/'+label)<<endl;
		  }
		}

		sBufor<<adres<<'/'<<git->label<<'/'<<git->counter++<<".jpg";
		sBufor>>cel;
		cerr<<cel<<endl;
	    
		Mat toWrite(rozm.width,rozm.height,CV_8U);
		resize(Mat(eq,(*it)),toWrite,toWrite.size(),
		       0,0,CV_INTER_LINEAR);

		imwrite(cel,toWrite);
	      
		git->photos.push_back(cel);
		git->pictures.push_back(toWrite.clone());
	      
	      }
	    }
	    if(label=="koniec")
	      break;
	  }
	}
      }
      if(ster!='q'){
	imshow("test",gemben);
	imshow("proces",mid);
	ster=waitKey(100);
      }
    }
    catch(Exception ex){
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
    }
    
  }

  //zapis galerii do pliku
  {
    FileStorage fs(string(adres+"/galeria.xml"),
		   FileStorage::WRITE);
    if(!fs.isOpened()){
      cv::Exception err(1,"file cannot be opened",
			__func__,__FILE__,__LINE__);
      throw err;
    }


    {
      fs<<GALERIES<<"[";
      for(list<Gallery>::iterator it=galeries.begin();
	  it!=galeries.end();++it){
	fs<<"{"<<LABEL<<it->label;
	fs<<COUNTER<<it->counter;
	fs<<ADDRES<<"[";
	for(unsigned i=0;i<it->photos.size();++i){
	  fs<<it->photos[i];
	}
	fs<<"]"<<"}";
      }

      fs<<"]";
    }
  }

  //koniec zapisu

  return 0;
}
