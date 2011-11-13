#include "Galleries.hpp"
#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <cstdlib>

using std::cerr;
using std::endl;
using namespace cv;

/*!
 * Constructor. Initialises path to galleries and loads galleries 
 * pointed by filename
 *
 * \param path - directory which contains or will contain galleries 
 * \param filename - name of file containig information about galleries
 *
 */

Galleries::Galleries(string path, string filename){
  _picSize.width=INITIAL_SIZE;
  _picSize.height=INITIAL_SIZE;
  _picType=CV_8U;
  setPath(path);
  load(filename);
}

int Galleries::getGalleryNumber(string label){
  int number=-1;
  for(number=0;number<_gal.size();++number){
    cerr<<"szuka "<<label<<" == "<<_gal[number].label<<'?'<<endl;
    if(_gal[number].label==label)
      break;
  }
  if(number==_gal.size()){
    return -1;
  }else{
    return number;
  }
}
  


/*!
 * Method loadnig galleries based on file pointed by filename
 *
 * \param filename - name of file based on which the galleries should be loaded
 *
 * \pre filename poins to a file generated by Galleries::save or containing 
 * informations from previously mentioned
 */ 

void Galleries::load(string filename){
  if(!_path.empty()){
    try{

      FileStorage fs(_path+'/'+filename,
		     FileStorage::READ);
      if(!fs.isOpened()){
	cv::Exception err(CANNOT_OPEN_FILE,
			  "file cannot be opened",
			  __func__,__FILE__,__LINE__);
	throw err;
      }
    
      FileNode galleries=fs[GALLERIES];
      FileNodeIterator it=galleries.begin();
      {
	int w,h;
	fs[WIDTH]>>w;
	fs[HEIGHT]>>h;
	_picSize=cv::Size(w,h);
      }

      for(;it!=galleries.end();++it){
	Gallery gallery;
	string label;
	(*it)[LABEL]>>label;
	int galNumber;
	
	galNumber=getGalleryNumber(label);
	
	if(galNumber<0){
	  galNumber=_gal.size();
	  _gal.push_back(Gallery());
	  (*it)[COUNTER]>>_gal.back().counter;
	  _gal.back().label=label;
	}else{
	  int cnt;
	  (*it)[COUNTER]>>cnt;
	  _gal[galNumber].counter+=cnt;
	}
// #if CV_MINOR_VERSION > 3
// 	(*it)[ADDRESS]>>gallery.photos;
// 	//	cerr<<_gal.label<<endl<<_gal.counter<<endl;
// 	{ //size check
// 	  unsigned i=0;
// 	  if(_picSize.width==INITIAL_SIZE && _picSize.height==INITIAL_SIZE){
// 	    cv::Mat img=imread(gallery.photos.front());
// 	    _picSize=img.size();
// 	    ++i;
// 	  }
// 	  for(;i<gallery.photos.size();++i){
// 	    cv::Mat img=imread(gallery.photos[i]);
// 	    if(img.size()!=_picSize){
// 	      Exception ex(NON_UNIFORM_GALLERY,
// 			   "exception: all images in the gallery must have the same dimensions",
// 			   __func__,__FILE__,__LINE__);
// 	      throw ex;
// 	    }
// 	  }
// 	}
// #else
	FileNode gfn=(*it)[ADDRESS];
	FileNodeIterator git=gfn.begin();
	for(;git!=gfn.end();++git){
	  _gal[galNumber].photos.push_back((string)(*git));
	  cv::Mat img=imread(_gal[galNumber].photos.back());
	  if(_picSize.width==INITIAL_SIZE && _picSize.height==INITIAL_SIZE){
	    _picSize=img.size();
	  }else{
	    if(img.size()!=_picSize){
	      Exception ex(NON_UNIFORM_GALLERY,
			   "exception: all images in the gallery must have the same dimensions",
			   __func__,__FILE__,__LINE__);
	      throw ex;
	    }
	  }
	}
    
	//#endif

	

    
      }
      fs.release();   
    }
    catch(Exception ex){
      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	  <<" in fucntion "<<__func__<<endl;
      throw ex;
    }
  }else{
    Exception ex(NO_PATH_DECLARED,
		 "exception: galleries path was not declared",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }
  
}


/*!
 * Method to add an image to a gallery. If gallery doesn't exist, it is created
 * 
 * \param label - label of gallery to which the image should be added
 * \param img - image to be added to gallery
 *
 * \pre Galleries are initialised and img has the same size and type as other 
 * images in the gallery
 */

void Galleries::add(string label,cv::Mat img){
  unsigned number;
  if(!_path.empty()){
    if(!_gal.empty()){
      for(number=0;number<_gal.size();++number){
	cerr<<"szuka "<<label<<" == "<<_gal[number].label<<'?'<<endl;
	if(_gal[number].label==label)
	  break;
      }


      if(_gal.size()==number){
	Gallery galeria;
	galeria.label=label;
	galeria.counter=0;
	_gal.push_back(galeria);
	if(!boost::filesystem::exists(_path+'/'+label)){
	  boost::filesystem::create_directory(_path+'/'+label);
	  cerr<<(_path+'/'+label)<<endl;
	}
      }
    }else{
      Gallery galeria;
      galeria.label=label;
      galeria.counter=0;
      _gal.push_back(galeria);
      number=0;
      _picSize=img.size();
      _picType=img.type();
      if(!boost::filesystem::exists(_path+'/'+label)){
	boost::filesystem::create_directory(_path+'/'+label);
	cerr<<(_path+'/'+label)<<endl;
      }
    }
    {
      if(img.size()!=_picSize){
	Exception ex(WRONG_PICTURE_SIZE,
		     "exception: picture size differs from standard galleries size",
		      __func__,__FILE__,__LINE__);
      }
	
      if(img.type()!=_picType){
	Exception ex(WRONG_PICTURE_TYPE,
		     "exception: picture type differs from standard galleries type",
		     __func__,__FILE__,__LINE__);
      }
      string cel;
      std::stringstream sBufor;
       sBufor<<_path<<'/'<<_gal[number].label<<'/'<<_gal[number].counter++
	     <<".jpg";
      sBufor>>cel;
      cerr<<cel<<endl;
      imwrite(cel,img);
      _gal[number].photos.push_back(cel);
    }
  }else{
    Exception ex(NO_PATH_DECLARED,
		 "exception: galleries path was not declared",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }
}

/*!
 * Method to set path to directory in which new gallery folders will be 
 * created and from which all relative paths to pictures start
 *
 * \param path - path to galleries directory
 *
 */

void Galleries::setPath(string path){
  if(!boost::filesystem::exists(path)){
    Exception ex(NO_SUCH_DIRECTORY,
		 "exception: directory "+path+"does not exist",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }else{
    _path=path;
  }
}

/*!
 * Method allowing to save galleries to a file 
 *
 * \param filename - name of xml file containing information aobut gallery and
 * localisation of image files
 */

void Galleries::save(string filename){
  if(!_path.empty()){
    FileStorage fs(_path+'/'+filename,
		   FileStorage::WRITE);
    if(!fs.isOpened()){
      cv::Exception err(CANNOT_OPEN_FILE,"file cannot be opened",
			__func__,__FILE__,__LINE__);
      throw err;
    }

  
    {
      fs<<GALLERIES<<"[";
      for(unsigned i=0;i<_gal.size();++i){
	fs<<"{"<<LABEL<<_gal[i].label;
	fs<<COUNTER<<_gal[i].counter;
	fs<<ADDRESS<<"[";
	for(unsigned j=0;j<_gal[i].photos.size();++j){
	  fs<<_gal[i].photos[j];
	}
	fs<<"]"<<"}";
      }
    
      fs<<"]";
      fs<<WIDTH<<_picSize.width<<HEIGHT<<_picSize.height<<TYPE<<_picType;
    }
  }else{
    Exception ex(NO_PATH_DECLARED,
		 "exception: galleries path was not declared",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }
}

/*!
 * Method alloing to retrieve a picture from galleries
 *
 * \param label - label of gallery from which image will be retrieved
 * \param number - numberofimage to be retrieved
 *
 * \return Image from gallery
 *
 * \pre Gallery with required label and photo with required number exist
 */

cv::Mat Galleries::getPicture(string label,int number){
  unsigned i;
  for(i=0;i<_gal.size();++i){
    if(label==_gal[i].label){
      try{
	return getPicture(i,number);
      }
      catch(cv::Exception ex){
	cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	    <<" in fucntion "<<__func__<<endl;
	throw ex;
      }
    }
  }
  cv::Exception ex(LABEL_NOT_FOUND,"exception: label not found",
		   __func__,__FILE__,__LINE__);
  throw ex;
}

/*!
 * Method alloing to retrieve a picture from galleries
 *
 * \param galleryNumber - label of gallery from which image will be retrieved
 * \param photoNumber - numberofimage to be retrieved
 *
 * \return Image from gallery
 *
 * \pre Gallery with required number and photo with required number exist
 */

cv::Mat Galleries::getPicture(int galleryNumber,int photoNumber){
  if(galleryNumber<0||galleryNumber>=_gal.size()){
    cv::Exception ex(INCORRECT_GALLERY_NUMBER,
		     "exception: incorrect gallery number",
		     __func__,__FILE__,__LINE__);
    throw ex;
  }else if(photoNumber<0||photoNumber>=_gal[galleryNumber].photos.size()){
    cv::Exception ex(INCORRECT_PHOTO_NUMBER,
		     "exception: incorrect photo number",
		     __func__,__FILE__,__LINE__);
    throw ex;
  }else{
    try{

      cv::Mat img=imread(_gal[galleryNumber].photos[photoNumber]);
      if(img.data==NULL){
	cv::Exception ex(CANNOT_OPEN_FILE,
			 "exception: could not open photo file",
		       __func__,__FILE__,__LINE__);
	throw ex;
      }
      return img;
    }
    catch(Exception ex){
      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	  <<" in fucntion "<<__func__<<endl;
      throw ex;
    }
  }
}

/*!
 * Method to check size of gallery
 *
 * \param galleryNumber - number of gallery which size will be checked
 *
 * \return size of gallery with required number
 *
 * \pre Gallery with required number exists
 */

int Galleries::gallerySize(int galleryNumber){
  if(galleryNumber<0||galleryNumber>_gal.size()){
    cv::Exception ex(INCORRECT_GALLERY_NUMBER,
		     "exception: incorrect gallery number",
		     __func__,__FILE__,__LINE__);
    throw ex;
  }else{
    return _gal[galleryNumber].photos.size();
  }
}

/*!
 * Method to retrieve label of gallery with required number
 *
 * \param galleryNumber - number of gallery to retrieve label from
 *
 * \return label of gallery
 *
 * \pre Gallery with galleryNumber exists
 */

std::string Galleries::getGalleryLabel(int galleryNumber){
  if(galleryNumber<0||galleryNumber>=_gal.size()){
    throw cv::Exception(INCORRECT_GALLERY_NUMBER,
			 "exception: incorrect gallery number",
			__func__,__FILE__,__LINE__);
  }
  return _gal[galleryNumber].label;
}

/*!
 * Method allows to randomly divide gallery into K roughly equal in numbers
 * subsets. The subsets are saved in xml files named "stub%", where % 
 * is a number. All photos will be distributed, and each photo will 
 * be part of only one subset. Usefull for cross validation tests.
 *
 * \param K - number of subsets
 * \param nameStub - name stub for subsets
 *
 */

void Galleries::createKSubsets(const int &K,const std::string &nameStub,
			       std::vector<std::string> &galleriesAddresses){
  int numberOfPhotos=0;
  int photosDistributed=0;
  int createdGalCnt=0;
  vector< vector<int> > usablePic;
  vector<int> usableGal;
  srand(time(NULL));

  usablePic.resize(totalSize());
  usableGal.resize(totalSize());
  for(int i=0;i<usablePic.size();++i){
    numberOfPhotos+=gallerySize(i);
    usablePic[i].resize(gallerySize(i));
    for(int j=0;j<usablePic[i].size();++j){
      usablePic[i][j]=j;
    }
    usableGal[i]=i;
  }
  
  while(numberOfPhotos>photosDistributed){
    Galleries tmp;
    tmp.setPath(_path);
    for(int photosInGallery=0;photosInGallery<=numberOfPhotos/K;
	++photosInGallery){
      int galNr=rand() % usablePic.size();
      int picNr=rand() % usablePic[galNr].size();

      tmp.add(getGalleryLabel(usableGal[galNr]),
	      getPicture(usableGal[galNr],usablePic[galNr][picNr]));
      usablePic[galNr].erase(usablePic[galNr].begin() + picNr);
      cerr<<++photosDistributed<<endl;
      if(usablePic[galNr].empty()){
	usablePic.erase(usablePic.begin() +galNr);
	usableGal.erase(usableGal.begin() + galNr);
      }
      if(usablePic.empty()){
	break;
      }
    }
    {    
      std::stringstream buff;
      string fileName;
      buff<<nameStub<<createdGalCnt++<<".xml";
      buff>>fileName;
      tmp.save(fileName);
      galleriesAddresses.push_back(fileName);
    }  
    cerr<<createdGalCnt<<endl;
  }

}
