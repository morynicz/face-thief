#include "SVMRec.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using std::string;
using std::vector;
using std::list;
using std::cerr;
using std::endl;

int SVMRec::POSITIVE=1;
int SVMRec::NEGATIVE=-1; 


// string SVMRec::DATA="DATA";
// string SVMRec::DATA_ROWS="DATA_ROWS";
// string SVMRec::DATA_COLS="DATA_COLS";
// string SVMRec::DATA_TYPE="DATA_TYPE";
string SVMRec::VECTORS="VECTORS";
//string SVMRec::ICOVAR="ICOVAR";
string SVMRec::VEC_ROWS="VECTORS_ROWS";
string SVMRec::VEC_COLS="VECTORS_COLS";
string SVMRec::VEC_TYPE="VECTORS_TYPE";
string SVMRec::LABEL_NR="LABEL_NR";
string SVMRec::EIGENVECTORS="EIGENVECTORS";
string SVMRec::EIGENVALUES="EIGENVALUES";
string SVMRec::EIGEN_ROWS="EIGEN_ROWS";
string SVMRec::EIGEN_COLS="EIGEN_COLS";
string SVMRec::EIGEN_TYPE="EIGEN_TYPE";
string SVMRec::MEAN="MEAN";
string SVMRec::MEAN_COLS="MEAN_COLS";
string SVMRec::MEAN_TYPE="MEAN_TYPE";
string SVMRec::SVMS="SVMS";
string SVMRec::SVMS_QUANTITY="SVMS_QUANTITY";
string SVMRec::SVM="SVM";


SVMRec::SVMRec(){
  name="SVM";
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

// void SVMRec::loadPrecomputedGalleries(const string& path){
//   clear();
//   try{
//     FileStorage fs(path,FileStorage::READ);
//     if(!fs.isOpened()){
//       cv::Exception err(CANNOT_OPEN_FILE,
// 			"file cannot be opened",
// 			__func__,__FILE__,__LINE__);
//       throw err;
//     }
    
//     // fs[DATA]>>_data;
//     fs[VECTORS]>>_vectors;
//     //fs[ICOVAR]>>_icovar;
//     fs[EIGENVECTORS]>>_pca.eigenvectors;
//     fs[EIGENVALUES]>>_pca.eigenvalues;
//     fs[MEAN]>>_pca.mean;


//     FileNode fn=fs[LABEL_NR];
//     for(FileNodeIterator it=fn.begin();it!=fn.end();++it){
//       _labelNr.push_back((int)(*it));
//     }
    
    
//     fs.release();
//   }
//   catch(Exception ex){
//     cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
// 	<<" in function "<<__func__<<endl;
//     throw ex;
//   }
// }

// void SVMRec::savePrecomputedGalleries(const string& path){
//   try{
//     FileStorage fs(path,FileStorage::WRITE);
//     if(!fs.isOpened()){
//       cv::Exception err(CANNOT_OPEN_FILE,
// 			"file cannot be opened",
// 			__func__,__FILE__,__LINE__);
//       throw err;
//     }
  

//     fs
//       //      <<DATA<<_data
//       <<VECTORS<<_vectors
//       //<<ICOVAR<<_icovar
//       <<EIGENVECTORS<<_pca.eigenvectors
//       <<EIGENVALUES<<_pca.eigenvalues
//       <<MEAN<<_pca.mean
//       <<LABEL_NR<<"[";
//     for(list<int>::iterator it=_labelNr.begin();
// 	it!=_labelNr.end();++it){
//       fs<<(*it);
//     }
//     fs<<"]";
//   }
//   catch(Exception ex){
//     cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
// 	<<" in function "<<__func__<<endl;
//     throw ex;
//   }
// }

void SVMRec::readFromBinary(cv::Mat &data,const string& path,Size size,int type){
  
    std::ifstream in(path.c_str(),std::ifstream::binary);
    
    //check size of file
    in.seekg(0,std::ifstream::end);
    int length=in.tellg();
    in.seekg(0);
      	//allocate
    char *buff=new char[length];
    in.read(buff,length);
    {
  
      Mat tmp(size,type,buff);
      data=tmp.clone();
    }    
    delete buff;
    in.close();
  
}

void SVMRec::loadPrecomputedGalleries(const string& path){
  clear();
  try{
    FileStorage fs(path,FileStorage::READ);
    if(!fs.isOpened()){
      cv::Exception err(CANNOT_OPEN_FILE,
			"file cannot be opened",
			__func__,__FILE__,__LINE__);
      throw err;
    }
    {
      string path;
      Mat tmp,tmp2;

      int rows,cols,type;
      // fs[DATA]>>path;
      // fs[DATA_ROWS]>>rows;
      // fs[DATA_COLS]>>cols;
      // fs[DATA_TYPE]>>type;
      // readFromBinary(_data,path,Size(cols,rows),type);
      fs[VECTORS]>>path;
      fs[VEC_ROWS]>>rows;
      fs[VEC_COLS]>>cols;
      fs[VEC_TYPE]>>type;
      readFromBinary(_vectors,path,Size(cols,rows),type);
      // fs[ICOVAR]>>path;
      // readFromBinary(_icovar,path,Size(cols,rows),type);
      fs[EIGENVECTORS]>>path;
      fs[EIGEN_ROWS]>>rows;
      fs[EIGEN_COLS]>>cols;
      fs[EIGEN_TYPE]>>type;
      readFromBinary(_pca.eigenvectors,path,Size(cols,rows),type);
      fs[EIGENVALUES]>>path;
      readFromBinary(_pca.eigenvalues,path,Size(1,rows),type);
      fs[MEAN]>>path;
      fs[MEAN_COLS]>>cols;
      fs[MEAN_TYPE]>>type;
      readFromBinary(_pca.mean,path,Size(cols,1),type);  
    }
    {
      FileNode fn=fs[LABEL_NR];
      for(FileNodeIterator it=fn.begin();it!=fn.end();++it){
	_labelNr.push_back((int)(*it));
      }
    }
    // {
    //   FileNode fn=fs[SVMS];
    //   string name;
    //   for(FileNodeIterator it=fn.begin();it!=fn.end();++it){
    // 	_svms.push_back(CvSVM());
    // 	name=(string)(*it);
    // 	_svms.back().load(name.c_str());
    //   }
    // }
    fs.release();
    
    {//SVM training
      for(int i=0;i<=_labelNr.back();++i){
    	vector<int> res;
    	for(list<int>::iterator it=_labelNr.begin();
    	    it!=_labelNr.end();++it){
    	  if((*it)==i){
    	    res.push_back(NEGATIVE);
    	  }else{
    	    res.push_back(POSITIVE);
    	  }
    	}
    	_svms.push_back(CvSVM());
    	_svms.back().train(_vectors,Mat(res));
    	//cerr<<_svms.length()<<endl;
    	//_svms.back().train_auto(_vectors,Mat(res),Mat(),Mat(),CvSVMParams());
	
      }
    }


  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  }
}

void SVMRec::writeToBinary(Mat &data,const string& path){
  std::ofstream out(path.c_str(),std::ofstream::binary);
  int bytes;
  switch(data.depth()){
  case CV_8U:
  case CV_8S:
    bytes=1;
    break;
  case CV_16U:
  case CV_16S:
    bytes=2;
    break;
  case CV_32F:
  case CV_32S:
    bytes=4;
    break;
  case CV_64F:
    bytes=8;
    break;
  default:
     cv::Exception err(UNKNOWN_MAT_TYPE,
			"Unknown Mat type, cannot write",
			__func__,__FILE__,__LINE__);
      throw err;
  }
  out.write((const char*)data.data,data.rows*data.cols*bytes
	    /*CV_32F->4*CV_8U*/);
  out.close();
}

void SVMRec::savePrecomputedGalleries(const string& path){
  try{
    FileStorage fs(path,FileStorage::WRITE);
    if(!fs.isOpened()){
      cv::Exception err(CANNOT_OPEN_FILE,
			"file cannot be opened",
			__func__,__FILE__,__LINE__);
      throw err;
    }
    fs
      // <<DATA_ROWS<<_data.rows
      // <<DATA_COLS<<_data.cols
      // <<DATA_TYPE<<_data.type()
      <<VEC_ROWS<<_vectors.rows
      <<VEC_COLS<<_vectors.cols
      <<VEC_TYPE<<_vectors.type()
      <<EIGEN_ROWS<<_pca.eigenvectors.rows
      <<EIGEN_COLS<<_pca.eigenvectors.cols
      <<EIGEN_TYPE<<_pca.eigenvectors.type()
      <<MEAN_COLS<<_pca.mean.cols
      <<MEAN_TYPE<<_pca.mean.type();
    {
      string name;
      string dir;
      string ext=".dat";
      {      
	size_t position=path.find_last_of("/");
	//if(position==string::npos){
	//   cv::Exception err(CANNOT_FIND_DIRECTORY,
	// 		    "file cannot be opened",
	// 		    __func__,__FILE__,__LINE__);
	//   throw err;
	// }
	dir=path.substr(0,position);
      }

      // name=dir+"/"+DATA+ext;
      // writeToBinary(_data,name);
      // fs<<DATA<<name;

      name=dir+"/"+VECTORS+ext;
      writeToBinary(_vectors,name);
      fs<<VECTORS<<name;

      // name=dir+"/"+ICOVAR+ext;
      // writeToBinary(_icovar,name);
      // fs<<ICOVAR<<name;

      name=dir+"/"+EIGENVECTORS+ext;
      writeToBinary(_pca.eigenvectors,name);
      fs<<EIGENVECTORS<<name;

      name=dir+"/"+EIGENVALUES+ext;
      writeToBinary(_pca.eigenvalues,name);
      fs<<EIGENVALUES<<name;

      name=dir+"/"+MEAN+ext;
      writeToBinary(_pca.mean,name);
      fs<<MEAN<<name;



      {
	int size=_svms.size();
	std::stringstream buff;
	fs<<SVMS_QUANTITY<<size
	  <<SVMS<<"[";
	list<CvSVM>::iterator iter=_svms.begin();
	for(int i=0;i<_svms.size();++i,++iter){
	  buff<<dir<<"/"<<SVM<<i<<".xml";
	  //	  name=dir+"/"+SVM+".xml";
	  buff>>name;
	  buff.clear();
	  cerr<<name<<endl;
	  iter->save(name.c_str());
	  fs<<name;
	}
	fs<<"]";
      }
    }    
    {
      fs<<LABEL_NR<<"[";
      for(list<int>::iterator it=_labelNr.begin();
	  it!=_labelNr.end();++it){
	fs<<(*it);
    }
      fs<<"]";
    }
    
    
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  } 
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

  
    for(int i=0;i<=_labelNr.back();++i){
      vector<int> res;
      for(list<int>::iterator it=_labelNr.begin();
	  it!=_labelNr.end();++it){
	if((*it)==i){
	  res.push_back(NEGATIVE);
	}else{
	  res.push_back(POSITIVE);
	}
      }
      _svms.push_back(CvSVM());
      _svms.back().train(_vectors,Mat(res));
      //cerr<<_svms.size()<<endl;
      //_svms.back().train_auto(_vectors,Mat(res),Mat(),Mat(),CvSVMParams());
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
  //int counter=0;
    
  try{
    if(img.channels()!=1){
      cvtColor(img,tmp,CV_RGB2GRAY);
    }else{
      tmp=img;
    }
    equalizeHist(tmp,eq);
    Result similarity;
    similarity.mean=0;
    similarity.min=0;
    similarity.max=0;
    similarity.label=-1;
   
    _pca.project(eq.reshape(1,1),vec);
    vec.convertTo(in,CV_32FC1);

    //cerr<<in.cols<<" "<<in.rows<<" "<<in.depth()<<" "<<in.channels()<<endl;
    //cerr<<_data.cols<<" "<<_data.rows<<" "<<_data.depth()<<" "<<_data.channels()<<endl;
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
  results.sort(compareMeanResults);
 return results;
}
  
SVMRec::~SVMRec(){

}

