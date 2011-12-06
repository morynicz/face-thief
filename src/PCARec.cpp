///\file
///\brief File containing implementation of class PCARec
///\author Michał Orynicz
#include "PCARec.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "FaceFactor.hpp"

using std::string;
using std::list;
using std::cerr;
using std::endl;
using namespace cv;
///\cond
string PCARec::DATA="DATA";
string PCARec::DATA_ROWS="DATA_ROWS";
string PCARec::DATA_COLS="DATA_COLS";
string PCARec::DATA_TYPE="DATA_TYPE";
string PCARec::VECTORS="VECTORS";
string PCARec::ICOVAR="ICOVAR";
string PCARec::VEC_ROWS="VECTORS_ROWS";
string PCARec::VEC_COLS="VECTORS_COLS";
string PCARec::VEC_TYPE="VECTORS_TYPE";
string PCARec::LABEL_NR="LABEL_NR";
string PCARec::EIGENVECTORS="EIGENVECTORS";
string PCARec::EIGENVALUES="EIGENVALUES";
string PCARec::EIGEN_ROWS="EIGEN_ROWS";
string PCARec::EIGEN_COLS="EIGEN_COLS";
string PCARec::EIGEN_TYPE="EIGEN_TYPE";
string PCARec::MEAN="MEAN";
string PCARec::MEAN_COLS="MEAN_COLS";
string PCARec::MEAN_TYPE="MEAN_TYPE";
///\endcond

/*!
 * \brief Constructor initialising name field
 */
PCARec::PCARec(){
  name="PCA";
}


/*!
 * Method loading galleries of images to internal structures of object
 *
 * \param galleries - object containing images that will be saved in this 
 * object
 *
 * \post Method compute may be used safely now
 */

void PCARec::loadGalleries(Galleries& galleries){
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
      Mat bw; //without this OCV2.2 would have a segmentation fault
	 
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

/*!
 * Method allowing to load previously computed PCA 
 *
 * \param target - target to xml file containing prevoiusly saved PCA
 *
 * \pre target points to a file created with PCARec::savePrecomputedGalleries or
 * containig the informations contained by previously mentioned
 */

void PCARec::loadPrecomputedGalleries(const string& target){
  clear();
  try{
    FileStorage fs(target,FileStorage::READ);
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
      fs[VECTORS]>>path;
      fs[VEC_ROWS]>>rows;
      fs[VEC_COLS]>>cols;
      fs[VEC_TYPE]>>type;
      readFromBinary(_vectors,path,Size(cols,rows),type);
      fs[ICOVAR]>>path;
      readFromBinary(_icovar,path,Size(cols,rows),type);
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
      {
	cerr<<_pca.eigenvectors.rows<<" "<<_pca.eigenvectors.cols<<endl;
	cerr<<FACE_FACTOR<<endl;
	Mat eigen;
	int width=sqrt(_pca.eigenvectors.cols/(1+FACE_FACTOR));
	int height=width+FACE_FACTOR*width;
	eigen.create(height*2,width*5,_pca.eigenvectors.type());
	for(int i=0;i<2;++i)
	  for(int j=0;j<5;++j){
	    Mat eigenface=eigen(Rect(j*width,i*height,width,height));
	    resize(_pca.eigenvectors.row(i*5+j).reshape(1,height),eigenface,
		   eigenface.size(),0,0,INTER_LINEAR);
	  }
	imwrite("eigenfaces.jpg",eigen*10000);
	imwrite("mean.jpg",_pca.mean.reshape(1,260));
      }
      
      
    }
    FileNode fn=fs[LABEL_NR];
    for(FileNodeIterator it=fn.begin();it!=fn.end();++it){
      _labelNr.push_back((int)(*it));
    }
    
    fs.release();
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  }
}


/*!
 * Method allowing to save computed PCA to files
 *
 * \param target - path and name of main PCA xml file
 *
 * \post Mean, eigenvectors,eigenvalues , inverted covariance matrix and 
 * pictures from gallery projected to PCA space saved in directory pointed by 
 * target
 */
void PCARec::savePrecomputedGalleries(const string& target){
  try{
    FileStorage fs(target,FileStorage::WRITE);
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
	size_t position=target.find_last_of("/");
	//if(position==string::npos){
	//   cv::Exception err(CANNOT_FIND_DIRECTORY,
	// 		    "file cannot be opened",
	// 		    __func__,__FILE__,__LINE__);
	//   throw err;
	// }
	dir=target.substr(0,position);
      }

      // name=dir+"/"+DATA+ext;
      // writeToBinary(_data,name);
      // fs<<DATA<<name;

      name=dir+"/"+VECTORS+ext;
      writeToBinary(_vectors,name);
      fs<<VECTORS<<name;

      name=dir+"/"+ICOVAR+ext;
      writeToBinary(_icovar,name);
      fs<<ICOVAR<<name;

      name=dir+"/"+EIGENVECTORS+ext;
      writeToBinary(_pca.eigenvectors,name);
      fs<<EIGENVECTORS<<name;

      name=dir+"/"+EIGENVALUES+ext;
      writeToBinary(_pca.eigenvalues,name);
      fs<<EIGENVALUES<<name;

      name=dir+"/"+MEAN+ext;
      writeToBinary(_pca.mean,name);
      fs<<MEAN<<name;

      fs<<LABEL_NR<<"[";
    }    
    for(list<int>::iterator it=_labelNr.begin();
	it!=_labelNr.end();++it){
      fs<<(*it);
    }
    fs<<"]";
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  } 
}

/*!
 * Method executes computations needed for projection and distance measurements
 * for PCARec class
 *
 * \pre Method loadGalleries was succesfully used to load galleries of images
 * to this object
 *
 * \post Object is ready to recognise
 */  

void PCARec::compute(){
  try{
    _pca(_data,Mat(),CV_PCA_DATA_AS_ROW);
    _pca.project(_data,_vectors);

    Mat covar;
    Mat mean;
      
    calcCovarMatrix(_vectors,covar,mean,
		    CV_COVAR_NORMAL|CV_COVAR_ROWS,
		    _vectors.type());
    invert(covar,_icovar,DECOMP_SVD);
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  } 
}

/*!
 * Method allowing to wipe data from object
 */

void PCARec::clear(){
  _labelNr.clear();
}

/*!
 * Method allows to use PCA to classify pattern on image pointed by target
 * 
 * \param target - image to be classified
 *
 * \return Ranked list of Result objects, sorted ascending by minimum value
 *
 * \pre Image pointed by target has the same number of pixels as images from 
 * galleries used for computing PCA. Compute, or loadPrecomputedGalleries 
 * method was succcesfully used
 */

std::list<Result> PCARec::recognise(const string& target){
  Mat img=imread(target);
  return recognise(img);
}

/*!
 * Method allows to use PCA to recognise data in img object
 *
 * \param img - object containing data to be classified
 *
 * \return Ranked list of Result objects, sorted ascending by minimum value
 *
 * \pre Img contains the same number of values as images from 
 * galleries used for computing PCA. Compute, or loadPrecomputedGalleries 
 * method was succcesfully used
 */

std::list<Result> PCARec::recognise(cv::Mat& img){
  Mat tmp,eq,vec;
  std::list<Result> results;
  std::list<int>::iterator it=_labelNr.begin();
  int counter=0;
  try{ //image preprocessing 
    if(img.channels()!=1){
      cvtColor(img,tmp,CV_RGB2GRAY);
    }else{
      tmp=img;
    }
    equalizeHist(tmp,eq);
    _pca.project(eq.reshape(1,1),vec);
  
    //recognition
    Result similarity;
    //    similarity.mean=0;
    // similarity.min=100;
    // similarity.max=0;
    similarity.score=100;
    similarity.label=-1;
    for(int i=0;i<_vectors.rows;++i){
      int label=*it; //computing distance
      double distance=Mahalanobis(_vectors.row(i),vec,_icovar);
      //similarity.score+=distance;
      //similarity.mean+=distance;
      // if(similarity.min>distance){ //searching extreme values
      // 	similarity.min=distance;
      // }
      if(similarity.score>distance){ //searching extreme values
      	similarity.score=distance;
       }
      // if(similarity.max<distance){
      // 	similarity.max=distance;
      // }
      ++it;
      ++counter;
      if(*it!=label||it==_labelNr.end()){//saving result
	//similarity.mean/=counter;
	similarity.label=label;
	results.push_back(similarity);
	// similarity.min=100;
	similarity.score=100;
	// similarity.max=0;
	//similarity.mean=counter=0;
	counter=0;
      }
    } 
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  } 
  results.sort(compareAscending);//sorting in ascending order by min value
  return results;
}

/*!
 * Destructor. Does nothing
 */

PCARec::~PCARec(){
  
}
