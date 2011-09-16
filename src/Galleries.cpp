#include "Galleries.hpp"
#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
#include "opencv2/highgui/highgui.hpp"

using std::cerr;
using std::endl;
using namespace cv;

Galleries::Galleries(string path, string filename){
  _picSize.width=INITIAL_SIZE;
  _picSize.height=INITIAL_SIZE;
  _picType=CV_8U;
  setPath(path);
  load(filename);
}

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
      
	(*it)[LABEL]>>gallery.label;
	(*it)[COUNTER]>>gallery.counter;

#if CV_MINOR_VERSION > 3
	(*it)[ADDRESS]>>gallery.photos;
	cerr<<_gal.label<<endl<<_gal.counter<<endl;
	{ //size check
	  int i=0;
	  if(_picSize.width==INITIAL_SIZE && _picSize.height==INITIAL_SIZE){
	    cv::Mat img=imread(gallery.photos.front());
	    _picSize=img.size();
	    ++i;
	  }
	  for(;i<gallery.photos.size();++i){
	    cv::Mat img=imread(gallery.photos[i]);
	    if(img.size()!=_picSize){
	      Exception ex(NON_UNIFORM_GALLERY,
			   "exception: all images in the gallery must have the same dimensions",
			   __func__,__FILE__,__LINE__);
	      throw ex;
	    }
	  }
	}
#else
	FileNode gfn=(*it)[ADDRESS];
	FileNodeIterator git=gfn.begin();
	for(;git!=gfn.end();++git){
	  
	  gallery.photos.push_back((string)(*git));
	  cerr<<gallery.photos.back()<<endl;
	  cv::Mat img=imread(gallery.photos.back());
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
    
#endif

	
	_gal.push_back(gallery);
    
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


// wczytywanie zakończone

//================ zapis
void Galleries::add(string label,cv::Mat img){
  if(!_path.empty()){
    vector<Gallery>::iterator git;
    if(!_gal.empty()){
      for(git=_gal.begin();
	  git!=_gal.end();
	  ++git){
	cerr<<"szuka "<<label<<" == "<<git->label<<'?'<<endl;
	if(git->label==label)
	  break;
      }
    
      if(git==_gal.end()){
	Gallery galeria;
	galeria.label=label;
	galeria.counter=0;
	--git;
	_gal.push_back(galeria);
	++git;
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
      git=_gal.begin();
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
      sBufor<<_path<<'/'<<git->label<<'/'<<git->counter++<<".jpg";
      sBufor>>cel;
      cerr<<cel<<endl;
      imwrite(cel,img);
      //gallery	      
      git->photos.push_back(cel);
    }
  }else{
    Exception ex(NO_PATH_DECLARED,
		 "exception: galleries path was not declared",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }
}


// ================ //dodawanie

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


//=================== zapis


//zapis galerii do pliku
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
      for(int i=0;i<_gal.size();++i){
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

cv::Mat Galleries::getPicture(string label,int number){
  int i;
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

cv::Mat Galleries::getPicture(int galleryNumber,int photoNumber){
  // cerr<<"cimcirimci"<<endl;
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
      //      cerr<<"tu byłem"<<endl;
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
