///\file
///\brief File containing implementation of class SVMRec
///\author Michał Orynicz
#include "SVMRec.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using std::cerr;
using std::endl;

///\cond
// Constants used for training
int SVMRec::POSITIVE=-1;
int SVMRec::NEGATIVE=1; 

//Constants used for saving and loading the object
string SVMRec::VECTORS="VECTORS";
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
///\endcond

/*!
 * \brief Constructor initialising name field
 */
SVMRec::SVMRec(){
  name="SVM";
}


/*!
 * Method loading galleries of images to internal structures of object
 *
 * \param galleries - object containing images that will be saved in this 
 * object
 *
 * \post Method compute may be used safely now
 */

void SVMRec::loadGalleries(Galleries& galleries){
  int rows=0;
  int cols=galleries.getPictureSize().width
    *galleries.getPictureSize().height;
  for(int i=0;i<galleries.totalSize();++i){
    rows+=galleries.gallerySize(i);
  }
  
  _data=cv::Mat(rows,cols,CV_8U);
  
  int y=0;
  for(int i=0;i<galleries.totalSize();++i){
    for(int j=0;j<galleries.gallerySize(i);++j){
      cv::Mat img=galleries.getPicture(i,j);
      cv::Mat bw; //needed or OCV2.2 would segment fault
      
      if(img.channels()!=1){
	cv::Mat tmp;
	cvtColor(img,tmp,CV_RGB2GRAY);
	equalizeHist(tmp,bw);
      }else{
	bw=img;
      }
      
      cv::Mat reshaped=bw.reshape(1,1);
      cv::Mat dataRow=_data.row(y++);
      resize(reshaped,dataRow,dataRow.size(),0,0,CV_INTER_LINEAR);
      _labelNr.push_back(i);
    }
  }
}

/*!
 * Method allowing to load previously computed SVMs 
 *
 * \param target - target to xml file containing prevoiusly saved SVMs
 *
 * \pre target points to a file created with SVMRec::savePrecomputedGalleries or
 * containig the informations contained by previously mentioned
 */

void SVMRec::loadPrecomputedGalleries(const string& target){
  clear();
  try{
    cv::FileStorage fs(target,cv::FileStorage::READ);
    if(!fs.isOpened()){
      cv::Exception err(CANNOT_OPEN_FILE,
			"file cannot be opened",
			__func__,__FILE__,__LINE__);
      throw err;
    }
    {
      string path;
      cv::Mat tmp,tmp2;

      int rows,cols,type;
      fs[VECTORS]>>path;
      fs[VEC_ROWS]>>rows;
      fs[VEC_COLS]>>cols;
      fs[VEC_TYPE]>>type;
      readFromBinary(_vectors,path,cv::Size(cols,rows),type);
      fs[EIGENVECTORS]>>path;
      fs[EIGEN_ROWS]>>rows;
      fs[EIGEN_COLS]>>cols;
      fs[EIGEN_TYPE]>>type;
      readFromBinary(_pca.eigenvectors,path,cv::Size(cols,rows),type);
      fs[EIGENVALUES]>>path;
      readFromBinary(_pca.eigenvalues,path,cv::Size(1,rows),type);
      fs[MEAN]>>path;
      fs[MEAN_COLS]>>cols;
      fs[MEAN_TYPE]>>type;
      readFromBinary(_pca.mean,path,cv::Size(cols,1),type);
    }
    {
      cv::FileNode fn=fs[LABEL_NR];
      for(cv::FileNodeIterator it=fn.begin();it!=fn.end();++it){
	_labelNr.push_back((int)(*it));
      }
    }
    {
      cv::FileNode fn=fs[SVMS];
      string name;
      for(cv::FileNodeIterator it=fn.begin();it!=fn.end();++it){
    	_svms.push_back(CvSVM());
    	name=(string)(*it);
    	_svms.back().load(name.c_str());
      }
    }
    fs.release();
    
  }
  catch(const cv::Exception &ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  }
}


/*!
 * Method allowing to save computed SVMs to files
 *
 * \param target - path and name of main SVMs xml file
 *
 * \post Mean, eigenvectors, eigenvalues of PCA space and SVMs saved in 
 * directory pointed by target
 */

void SVMRec::savePrecomputedGalleries(const string& target){
  try{
    cv::FileStorage fs(target,cv::FileStorage::WRITE);
    if(!fs.isOpened()){
      cv::Exception err(CANNOT_OPEN_FILE,
			"file cannot be opened",
			__func__,__FILE__,__LINE__);
      throw err;
    }
    fs
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
	size_t position=target.find_last_of("/");
	dir=target.substr(0,position);
      }


      name=dir+"/"+VECTORS+ext;
      writeToBinary(_vectors,name);
      fs<<VECTORS<<name;

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
	std::list<CvSVM>::iterator iter=_svms.begin();
	for(int i=0;i<_svms.size();++i,++iter){
	  buff<<dir<<"/"<<SVM<<i<<".xml";
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
      for(std::list<int>::iterator it=_labelNr.begin();
	  it!=_labelNr.end();++it){
	fs<<(*it);
    }
      fs<<"]";
    }
    
    
  }
  catch(const cv::Exception &ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  } 
}


/*!
 * Method executes computations needed for projection into PCA space and 
 * training SVMs
 *
 * \pre Method loadGalleries was succesfully used to load galleries of images
 * to this object
 *
 * \post Object is ready to recognise
 */

void SVMRec::compute(){
  try{
    {
      cv::Mat tmp;
      _data.convertTo(tmp,CV_32FC1);
      _data=tmp;
    }
    
    _pca(_data,cv::Mat(),CV_PCA_DATA_AS_ROW);
    _pca.project(_data,_vectors);

    {
      CvSVMParams params;
      params.svm_type=CvSVM::C_SVC;
      params.kernel_type=CvSVM::POLY;
      params.C=1.1;
      params.coef0=1.1;
      params.degree=2;
      params.gamma=1.1;
      params.term_crit =  cv::TermCriteria(CV_TERMCRIT_ITER, (int)1e4, 1e-4);
      for(int i=0;i<=_labelNr.back();++i){
	std::vector<int> res;
	for(std::list<int>::iterator it=_labelNr.begin();
	    it!=_labelNr.end();++it){
	  if((*it)==i){
	    res.push_back(POSITIVE);
	  }else{
	    res.push_back(NEGATIVE);
	  }
	}
	cv::Mat tmp=_vectors.clone();
	_svms.push_back(CvSVM());
	cerr<<_svms.size()<<endl;
	if(!_svms.back().train_auto(tmp,cv::Mat(res),cv::Mat(),cv::Mat(),params)){
	  cv::Exception ex(SVM_TRAINING_FAILURE,
			    "svm could not find a solution",
			    __func__,__FILE__,__LINE__);
	  throw ex;
	}
	cerr<<"C: "<<params.C<<endl;
	cerr<<"coef0: "<<params.coef0<<endl;
	cerr<<"gamma: "<<params.gamma<<endl;
	cerr<<"degree: "<<params.degree<<endl;
      }
    }
  }
  catch(const cv::Exception &ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  } 
}


/*!
 * Method allowing to wipe data from object
 */

void SVMRec::clear(){
  _svms.clear();
}

/*!
 * Method allows to use SVMRec to classify pattern on image pointed by target
 * 
 * \param target - image to be classified
 *
 * \return Ranked list of Result objects, sorted descending by mean value
 *
 * \pre Image pointed by target has the same number of pixels as images from 
 * galleries used for computing SVMs and PCA. Compute, or 
 * loadPrecomputedGalleries method was succcesfully used
 */

std::list<Result> SVMRec::recognise(const string& target){
  cv::Mat img=cv::imread(target);
  return recognise(img);
}
  

/*!
 * Method allows to use SVMRec to recognise data in img object
 *
 * \param img - object containing data to be classified
 *
 * \return Ranked list of Result objects, sorted descending by mean value
 *
 * \pre Img contains the same number of values as images from 
 * galleries used for computing PCA and SVMs. Compute, or 
 * loadPrecomputedGalleries method was succcesfully used
 */

std::list<Result> SVMRec::recognise(cv::Mat& img){

  cv::Mat tmp,eq,vec,in;
  std::list<Result> results;
  std::list<int>::iterator lit=_labelNr.begin();
    
  try{//image preprocessing
    if(img.channels()!=1){
      cvtColor(img,tmp,CV_RGB2GRAY);
    }else{
      tmp=img;
    }
    equalizeHist(tmp,eq);
    Result similarity;
    similarity.score=0;
    similarity.label=-1;
    _pca.project(eq.reshape(1,1),vec);
    int cnt=0; //querying SVMs with the image
    for(std::list<CvSVM>::iterator it=_svms.begin();
	it!=_svms.end();++it,++cnt){
      similarity.score=it->predict(vec,true);
      similarity.label=cnt;
      results.push_back(similarity);
      similarity.score=0;
      similarity.label=-1;
    }
  }
  catch(const cv::Exception &ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  } 
  results.sort(compareDescending); //sorting results in descending order by
  // mean value
 return results;
}
  
/*!
 * Destructor. Does nothing
 */

SVMRec::~SVMRec(){

}

 
