#include "PPRec.hpp"
#include "ocv2pit.hpp"
#include <vector>

using namespace cv;
using std::string;
using std::vector;
using std::list;
// TRY
// PPR_SUCCESS
// PPR_REQUIRES_FRONTAL_FACE_OBJECT
// ppr_error_message(err);
// exit(EXIT_FAILURE);

void eC(ppr_error_type err){
  if(err!= PPR_SUCCES){
    Exception ex(PITTPATT_ERROR,ppr_error_message(err),__func__,__FILE__,
		 __LINE__);
    throw ex;
  }
}

PPRec::PPRec(){
  modelPath="../../pittpatt/pittpatt_sdk/models";
  galleryFile="galleries.ppr";
  precision=PPR_FINE_PRECISION;
  detektor=PPR_NO_LANDMARK_DETECTOR;
  threadNumber=1;
  threadRecognitionNumber=1;
  searchPrunning=PPR_MAX_SEARCH_PRUNNING_AGGRESIVENESS;
  minSize=4;
  maxSize=15;
  adaptiveMinSize=0.01;
  adaptiveMaxSize=1.0;
  detectionThreshold=0.0;
  yawConstraint=PPR_FRONTAL_YAW_CONSTRAINT_RESTRICTIVE;
  templateExtractor=PPR_EXTRACT_SINGLE;
    

  context=ppr_get_context();
  try{  
    eC(ppr_enable_tracking(context));
    eC(ppr_enable_recognition(context));
  
    eC(ppr_set_detection_precission(context,precision));
    eC(ppr_set_num_detection_threads(context,threadNumber));
    eC(ppr_set_search_prunning_aggressiveness(context,searchPrunning));
    eC(ppr_set_num_recognition_threads(context,threadRecognitionNumber));
    eC(ppr_set_min_size(context, minSize));
    eC(ppr_set_max_size(context, maxSize));
    eC(ppr_set_template_extraction_type(context,templateExtractor));

    eC(ppr_initialize_context(context));
 
    eC(ppr_create_gallery(context,&pGallery));
 }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in fucntion "<<__func__<<endl;
    throw ex;
  }
    
}

void loadGalleries(Galleries& galleries){
  ppr_image_type pImg;
  ppr_object_list_type oList;
  ppr_subject_list_type sList;
  ppr_tracked_object_list tList;
  ppr_template_type pTemplate;
  ppr_object_suitability_type recAble;
  
  vector<int> idList;
  vector<int>lList;

  int rows=0;
  int cols=galleries.getPictureSize().width
    *galleries.getPictureSize().height;
  for(int i=0;i<galleries.totalSize();++i){
    rows+=galleries.gallerySize(i);
  }
  
  //_data=Mat(rows,cols,CV_8U);
  try{  
    int y=0;
    for(int i=0;i<galleries.totalSize();++i){
      for(int j=0;j<galleries.gallerySize(i);++j){
	Mat img=galleries.getPicture(i,j);
	// Mat bw; //needed or OCV2.2 would segment fault
	 
	// if(img.channels()!=1){
	// 	Mat tmp;
	// 	cvtColor(img,tmp,CV_RGB2GRAY);
	// 	equalizeHist(tmp,bw);
	// }else{
	// 	bw=img;
	// }
	 
	// Mat reshaped=bw.reshape(1,1);
	// Mat dataRow=_data.row(y++);
	// resize(reshaped,dataRow,dataRow.size(),0,0,CV_INTER_LINEAR);
	// _labelNr.push_back(i);
	eC(mat2PprImage(img,pImg,PPR_RAW_IMAGE_GRAY8));
	eC(ppr_detect_objects(context,pImg,&oList));
	eC(ppr_detect_landmarks(context,pImg,&oList)); //might be unnecessary
	for(int k=0;k<oList.numObjects;++k){
	  int id;
	  eC(ppr_is_object_suitable_for_recognition(context,oList.objects[k],
						    &recAble));
	  if(PPR_OBJECT_SUITABLE_FOR_RECOGNITNION==recAble){
	    eC(ppr_extract_template_from_object(context,pImage,oList.objects[k],
						&pTemplate));
	    eC(ppr_set_template_string(context,&pTemplate,
				       galleries.getGalleryLabel(i).c_str()));
	    eC(ppr_copy_template_to_gallery(context,&pGallery,pTemplate,&id));
	    ppr_free_template(pTemplate);
	    idList.push_back(id);
	    lList.push_back(i);
	  }
	}
      }
    }
    for(int i=1;i<idList.size();++i){
      if(lList[i-1]!=lList[i]){
	ec(ppr_set_template_relationship(context,&gallery,idList[i-1],idList[i],
					 PPR_RELATIONSHIP_DIFFERENT_SUBJECTS));
      }else{
	ec(ppr_set_template_relationship(context,&gallery,idList[i-1],idList[i],
					 PPR_RELATIONSHIP_SAME_SUBJECT));
      }
    }
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in fucntion "<<__func__<<endl;
    throw ex;
  }
}

void loadPrecomputedGalleries(const string& path){
  
}
  
void savePrecomputedGalleries(const string& path){
  
}

void compute(){
  //all is done in load gallery
}

void clear(){

}

list<Result> recognise(const string& path){
  try{
    Mat img;
    img=imread(path);
    return recognise(img);
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in fucntion "<<__func__<<endl;
    throw ex;
  }
}

list<Result> recognise(Mat &img){
  ppr_image_type pImage;
  
  eC
  
}

PPRec::~PPRec(){
  ppr_finalize_sdk();
}
    
