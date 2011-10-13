#include "PCARec.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
using std::string;
using std::list;
using std::cerr;
using std::endl;
using namespace cv;

string PCARec::DATA="DATA";
string PCARec::VECTORS="VECTORS";
string PCARec::ICOVAR="ICOVAR";
string PCARec::LABEL_NR="LABEL_NR";
string PCARec::EIGENVECTORS="EIGENVECTORS";
string PCARec::EIGENVALUES="EIGENVALUES";
string PCARec::MEAN="MEAN";

PCARec::PCARec(){
 
}

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

void PCARec::loadPrecomputedGalleries(const string& path){
  clear();
  try{
    FileStorage fs(path,FileStorage::READ);
    if(!fs.isOpened()){
      cv::Exception err(CANNOT_OPEN_FILE,
			"file cannot be opened",
			__func__,__FILE__,__LINE__);
      throw err;
    }
    
    fs[DATA]>>_data;
    fs[VECTORS]>>_vectors;
    fs[ICOVAR]>>_icovar;
    fs[EIGENVECTORS]>>_pca.eigenvectors;
    fs[EIGENVALUES]>>_pca.eigenvalues;
    fs[MEAN]>>_pca.mean;


    FileNode fn=fs[LABEL_NR];
    for(FileNodeIterator it=fn.begin();it!=fn.end();++it){
      _labelNr.push_back((int)(*it));
    }
    
    
    fs.release();
    // _pca(_data,Mat(),CV_PCA_DATA_AS_ROW);
    cerr<<_data.cols<<" "<<_data.rows<<endl;
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in fucntion "<<__func__<<endl;
    throw ex;
  }
}

void PCARec::savePrecomputedGalleries(const string& path){
  try{
    FileStorage fs(path,FileStorage::WRITE);
    if(!fs.isOpened()){
      cv::Exception err(CANNOT_OPEN_FILE,
			"file cannot be opened",
			__func__,__FILE__,__LINE__);
      throw err;
    }
  
    //    cerr<<"data"<<endl;
    fs
      <<DATA<<_data
      <<VECTORS<<_vectors
      <<ICOVAR<<_icovar
      <<EIGENVECTORS<<_pca.eigenvectors
      <<EIGENVALUES<<_pca.eigenvalues
      <<MEAN<<_pca.mean
      <<LABEL_NR<<"[";
    //    cerr<<"labels"<<endl;
    for(list<int>::iterator it=_labelNr.begin();
	it!=_labelNr.end();++it){
      fs<<(*it);
      //      cerr<<z++<<endl;
    }
    fs<<"]";
    // cerr<<"post"<<endl;
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in fucntion "<<__func__<<endl;
    throw ex;
  } 
}
  
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
	<<" in fucntion "<<__func__<<endl;
    throw ex;
  } 
}

void PCARec::clear(){
  _labelNr.clear();
}

std::list<Result> PCARec::recognise(const string& path){
  Mat img=imread(path);
  return recognise(img);
}

std::list<Result> PCARec::recognise(cv::Mat& img){
  Mat tmp,eq,vec;
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
    _pca.project(eq.reshape(1,1),vec);
  
 
    Result similarity;
    similarity.mean=0;
    similarity.min=100;
    similarity.max=0;
    similarity.label=-1;
    for(int i=0;i<_vectors.rows;++i){
      int label=*it;
      double distance=Mahalanobis(_vectors.row(i),vec,_icovar);
      //cerr<<label<<" "<<distance<<endl;
      similarity.mean+=distance;
      if(similarity.min>distance){
	similarity.min=distance;
      }
      if(similarity.max<distance){
	similarity.max=distance;
      }
      ++it;
      ++counter;
      if(*it!=label||it==_labelNr.end()){
	similarity.mean/=counter;
	similarity.label=label;
	results.push_back(similarity);
	similarity.min=100;
	similarity.max=0;
	similarity.mean=counter=0;
      }
    } 
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in fucntion "<<__func__<<endl;
    throw ex;
  } 
  return results;
}

PCARec::~PCARec(){
  
}

/*  
  {//PCA
    { //Odczyt i przejście na tablice wektorow
      int rows=0;
      int cols=galleries.getPictureSize().width
	*galleries.getPictureSize().height;
      for(int i=0;i<galleries.totalSize();++i){
	rows+=galleries.gallerySize(i);
      }
      
      Mat input(rows,cols,CV_8U);
      
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
	  Mat inInput=input.row(y++);
	  resize(reshaped,inInput,inInput.size(),0,0,CV_INTER_LINEAR);
	
	}
      }
      
    
      int maxComponents=cols*0;
      cerr<<maxComponents<<endl;
      PCA pca(input,Mat(),CV_PCA_DATA_AS_ROW,maxComponents);
   
      //      Mat compressed(rows,maxComponents,CV_8U);
      Mat compressed;
      pca.project(input,compressed);


      

	 
	 cerr<<do_golenia.cols<<do_golenia.rows<<do_golenia.channels()<<endl;

	 Mat ch,ch2;
	 cvtColor(do_golenia,ch2,CV_RGB2GRAY);
	 
      cerr<<ch2.type()<<endl;
      cerr<<ch2.cols<<" "<<ch2.rows<<endl;
      cerr<<input.cols<<endl;
      pca.project(ch2.reshape(1,1),kompresowany);



      cerr<<compressed.cols<<" "<<compressed.rows<<endl;
      cerr<<compressed.depth()<<compressed.channels()<<endl;
      cerr<<pca.eigenvectors.cols<<" "<<pca.eigenvectors.rows<<endl;
      Mat reconstructed;

      pca.backProject(compressed,reconstructed);

      // 
      // for(int i=0;i<rows;++i){
      // 	Mat img=reconstructed.row(i).reshape(0,galleries.getPictureSize().width);
      // 	Mat umg=input.row(i).reshape(0,galleries.getPictureSize().width);
      // 	Mat emg=pca.eigenvectors.row(i).reshape(0,galleries.getPictureSize().width);
	
      // 	imshow("in",img);
      // 	imshow("proces",umg);
      // 	imshow("test",emg*100);
      // 	waitKey(5100);

      // }
      // 

      Mat covar;
      Mat mean;
      Mat icovar;
      
      calcCovarMatrix(compressed,covar,mean,
		      CV_COVAR_NORMAL|CV_COVAR_ROWS,
		      compressed.type());
      invert(covar,icovar,DECOMP_SVD);

      for(int i=0;i<rows;++i){
	 double dist2=Mahalanobis(compressed.row(i),kompresowany,icovar.t());
	 cerr<<i<<" "<<dist2<<endl<<endl;
	for(int j=0;j<rows;++j){
	  Mat in[2];//coord i , j
	 
	  in[0]=compressed.row(i);
	  in[1]=compressed.row(j);
	 
	  double dist=Mahalanobis(in[0],in[1],icovar.t());
	 
	  cerr<<i<<" "<<j<<" "<<dist<<endl;
	
	
	}
      }
      
    }
  }
*/
