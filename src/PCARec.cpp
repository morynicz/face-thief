#include "PCARec.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using std::string;
using namespace cv;

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

}

void PCARec::savePrecomputedGalleries(const string& path){

}

void PCARec::compute(){
  _pca(_data,Mat(),CV_PCA_DATA_AS_ROW);
  _pca.project(_data,_vectors);


  Mat covar;
  Mat mean;
      
  calcCovarMatrix(_vectors,covar,mean,
		  CV_COVAR_NORMAL|CV_COVAR_ROWS,
		  _vectors.type());
  invert(covar,_icovar,DECOMP_SVD);
}

void PCARec::clear(){

}

std::list<Result> PCARec::recognise(const string& path){

}

std::list<Result> PCARec::recognise(cv::Mat& img){
  Mat tmp,eq,vec;
  std::list<Result> results;
  std::list<int>::iterator it=_labelNr.begin();
  int counter=0;
  if(img.channels()!=1){
    cvtColor(img,tmp,CV_RGB2GRAY);
  }else{
    tmp=img;
  }
  equalizeHist(tmp,eq);
  _pca.project(eq.reshape(1,1),vec);
  
 
  Result similarity;
  similarity.score=0;
  similarity.label=-1;
  for(int i=0;i<_vectors.cols;++i){
    int label=*it;
    similarity.score+=Mahalanobis(_vectors.row(i),vec,_icovar);
    ++it;
    ++counter;
    if(*it!=label){
      similarity.score/=counter;
      similarity.label=label;
      results.push_back(similarity);
      similarity.score=counter=0;
    }
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
