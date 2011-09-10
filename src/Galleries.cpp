#include "Galleries.hpp"

void Galleries::load(string filename){

  try{

    FileStorage fs(string(filename),
		   FileStorage::READ);
    if(!fs.isOpened()){
      cv::Exception err(CANNOT_OPEN_FILE,
			"file cannot be opened",
			__func__,__FILE__,__LINE__);
      throw err;
    }
    
    FileNode gals=fs["Galeries"];
    FileNodeIterator it=gals.begin();
    
    for(;it!=gals.end();++it){
      Gallery gal;
      
      (*it)[LABEL]>>gal.label;
      (*it)[COUNTER]>>gal.counter;

#if CV_MINOR_VERSION > 3
      (*it)[ADDRES]>>gal.photos;
      cerr<<gal.label<<endl<<gal.counter<<endl;
      /*  for(unsigned i=0;i<gal.photos.size();++i){
	  Mat img;
	  cerr<<gal.photos[i]<<endl;
	  img=imread(gal.photos[i]);
	  gal.pictures.push_back(img);*/
    }
#else
    FileNode gfn=(*it)[ADDRES];
    FileNodeIterator git=gfn.begin();
    for(;git!=gfn.end();++git){
      Mat img;
      gal.photos.push_back((string)(*git));
      cerr<<gal.photos.back()<<endl;
      /*	img=imread(gal.photos.back());
		gal.pictures.push_back(img);*/
    }

#endif
    galeries.push_back(gal);
      
  }
   
  fs.release();   
} 


  // wczytywanie zakończone

//================ zapis
void Galleries::add(string label,cv::Mat img){
  vector<Gallery>::iterator git;
  if(!galeries.empty()){
    for(git=gal.begin();
	git!=gal.end();
	++git){
      cerr<<"szuka "<<label<<" == "<<git->label<<'?'<<endl;
      if(git->label==label)
	break;
    }
    
    if(git==galeries.end()){
      Gallery galeria;
      galeria.label=label;
      galeria.counter=0;
      --git;
      gal.push_back(galeria);
      ++git;
      if(!boost::filesystem::exists(adres+'/'+label)){
		      boost::filesystem::create_directory(adres+'/'+label);
		      cerr<<(adres+'/'+label)<<endl;
      }
    }
  }else{
    Gallery galeria;
    galeria.label=label;
    galeria.counter=0;
    gal.push_back(galeria);
    git=gal.begin();
    if(!boost::filesystem::exists(adres+'/'+label)){
      boost::filesystem::create_directory(adres+'/'+label);
      cerr<<(adres+'/'+label)<<endl;
    }
  }
  
  sBufor<<adres<<'/'<<git->label<<'/'<<git->counter++<<".jpg";
  sBufor>>cel;
  cerr<<cel<<endl;
	    
  //gallery	      
  git->photos.push_back(cel);
  git->pictures.push_back(toWrite.clone());
  
}


// ================ //dodawanie

//=================== zapis


  //zapis galerii do pliku
void Galleries::save(string filename){
  FileStorage fs(filename,
		 FileStorage::WRITE);
  if(!fs.isOpened()){
    cv::Exception err(CANNOT_OPEN_FILE,"file cannot be opened",
		      __func__,__FILE__,__LINE__);
    throw err;
  }


  {
    fs<<GALERIES<<"[";
    for(int i=0;i<gal.size();++i){
      it!=galeries.end();++it){
      fs<<"{"<<LABEL<<it->label;
      fs<<COUNTER<<it->counter;
      fs<<ADDRES<<"[";
      for(unsigned j=0;j<gal[i].photos.size();++j){
	fs<<gal[i].photos[j];
      }
      fs<<"]"<<"}";
    }

    fs<<"]";
  }
}

