#include "SVMRec.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using std::string;
using std::vector;
using std::list;
using std::cerr;
using std::endl;

int SVMRec::POSITIVE=1;
int SVMRec::NEGATIVE=-1; 

SVMRec::SVMRec(){

}

void SVMRec::loadGalleries(Galleries& galleries){
  int rows=0;
  int cols=galleries.getPictureSize().width
    *galleries.getPictureSize().height;
  for(int i=0;i<galleries.totalSize();++i){
    rows+=galleries.gallerySize(i);
  }
  
  _data=Mat(rows,cols,CV_8U);
  
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
      Mat dataRow=_data.row(y++);
      resize(reshaped,dataRow,dataRow.size(),0,0,CV_INTER_LINEAR);
      _labelNr.push_back(i);
    }
  }
}

void SVMRec::loadPrecomputedGalleries(const string& path){

}

void SVMRec::savePrecomputedGalleries(const string& path){

}

void SVMRec::compute(){
  try{
    {
      Mat tmp;
      _data.convertTo(tmp,CV_32FC1);
      _data=tmp;
    }
    
    _pca(_data,Mat(),CV_PCA_DATA_AS_ROW);
    _pca.project(_data,_vectors);

  
    for(int i=0;i<_labelNr.back();++i){
      vector<int> res;
      for(list<int>::iterator it=_labelNr.begin();
	  it!=_labelNr.end();++it){
	if((*it)!=i){
	  res.push_back(NEGATIVE);
	}else{
	  res.push_back(POSITIVE);
	}
      }
      _svms.push_back(CvSVM());
      _svms.back().train(_vectors,Mat(res));
    }
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in fucntion "<<__func__<<endl;
    throw ex;
  } 
}

void SVMRec::clear(){
  _svms.clear();
}

list<Result> SVMRec::recognise(const string& path){
  Mat img=imread(path);
  return recognise(img);
}
  
list<Result> SVMRec::recognise(Mat& img){

  Mat tmp,eq,vec,in;
  std::list<Result> results;
  std::list<int>::iterator it=_labelNr.begin();
  int counter=0;
    
  try{
    if(img.channels()!=1){
      cvtColor(img,tmp,CV_RGB2GRAY);
    }else{
      tmp=img;
    }
    equalizeHist(tmp,eq);
    Result similarity;
    similarity.mean=0;
    similarity.min=100;
    similarity.max=0;
    similarity.label=-1;
   
    _pca.project(eq.reshape(1,1),vec);
    vec.convertTo(in,CV_32FC1);

    cerr<<in.cols<<" "<<in.rows<<" "<<in.depth()<<" "<<in.channels()<<endl;
    cerr<<_data.cols<<" "<<_data.rows<<" "<<_data.depth()<<" "<<_data.channels()<<endl;
    int cnt=0;
    for(list<CvSVM>::iterator it=_svms.begin();
	it!=_svms.end();++it,++cnt){
      similarity.mean=it->predict(in.reshape(1,1),true);
      similarity.label=cnt;
      results.push_back(similarity);
      similarity.mean=0;
      similarity.min=100;
      similarity.max=0;
      similarity.label=-1;
    }
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in fucntion "<<__func__<<endl;
    throw ex;
  } 
    

  // _pca.project(eq.reshape(1,1),vec);
  
 

  // similarity.mean=0;
  // similarity.min=100;
  // similarity.max=0;
  // similarity.label=-1;
  // for(int i=0;i<_vectors.rows;++i){
  //   int label=*it;
  //   double distance=Mahalanobis(_vectors.row(i),vec,_icovar);
  //   cerr<<label<<" "<<distance<<endl;
  //   similarity.mean+=distance;
  //   if(similarity.min>distance){
  //     similarity.min=distance;
  //   }
  //   if(similarity.max<distance){
  //     similarity.max=distance;
  //   }
  //   ++it;
  //   ++counter;
  //   if(*it!=label||it==_labelNr.end()){
  //     similarity.mean/=counter;
  //     similarity.label=label;
  //     results.push_back(similarity);
  //     similarity.min=100;
  //     similarity.max=0;
  //     similarity.mean=counter=0;
  //   }
  // }
  return results;
}
  
SVMRec::~SVMRec(){

}

