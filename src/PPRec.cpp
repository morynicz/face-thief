///\file
///\brief File containing implementation of class PPRec
///\author Michał Orynicz
#include "PPRec.hpp"
#include "ocv2pit.hpp"
#include <vector>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#ifdef PITTPATT_PRESENT
#include <pittpatt/pittpatt_license.h>
#include <cstdio>

using namespace cv;
using std::string;
using std::vector;
using std::list;
using std::cerr;
using std::endl;

/*!
 * Assertion method. If ppr operation was not succesfull, throws proper
 * exception
 *
 * \param err - value returned by ppr function
 * \param func - meant to recieve __func__
 * \param file - meant to recieve __FILE__
 * \param line - meant to recieve __LINE__
 *
 * By using func, file and line, exception thrown contains information about
 * function in which unsuccesfull operation ocured, not about this function.
 *
 */

void PPRec::eC(ppr_error_type err,string func,string file,int line){
  if(err!= PPR_SUCCESS){
    Exception ex(PITTPATT_ERROR,ppr_error_message(err),func,file,line);
    throw ex;
  }
}


/*!
 * Constructor initialising name field
 */

PPRec::PPRec(){
  name="PPR";
  initialised=loaded=false;
}

/*!
 * Function initialising the whole structure, separate from constructor to let 
 * the initialisation moment be delayed
 *
 */

void PPRec::initialize(){
  modelPath="../../pittpatt/pittpatt_sdk/models/";
  galleryFile="galleries.ppr";
  precision=PPR_FINE_PRECISION;
  detector=PPR_DUAL_FRONTAL_LANDMARK_DETECTOR;
  detectorMode=PPR_AUTOMATIC_LANDMARKS;
  threadNumber=1;
  threadRecognitionNumber=1;
  searchPrunning=PPR_MAX_SEARCH_PRUNING_AGGRESSIVENESS;
  minSize=4;
  maxSize=15;
  adaptiveMinSize=0.01;
  adaptiveMaxSize=1.0;
  detectionThreshold=0.0;
  yawConstraint=PPR_FRONTAL_YAW_CONSTRAINT_RESTRICTIVE;
  templateExtractor=PPR_EXTRACT_SINGLE;
    
  context=ppr_get_context();
  try{  
  
    eC(ppr_set_license(context,my_license_id,my_license_key),
       __func__,__FILE__,__LINE__);

    //eC(ppr_enable_detection(context),__func__,__FILE__,__LINE__);
    eC(ppr_enable_recognition(context),__func__,__FILE__,__LINE__);
    eC(ppr_set_models_path(context,modelPath.c_str()),
       __func__,__FILE__,__LINE__);
    eC(ppr_set_detection_precision(context,precision),
       __func__,__FILE__,__LINE__);
    eC(ppr_set_num_detection_threads(context,threadNumber),
       __func__,__FILE__,__LINE__);
    eC(ppr_set_search_pruning_aggressiveness(context,searchPrunning),
       __func__,__FILE__,__LINE__);
    eC(ppr_set_num_recognition_threads(context,threadRecognitionNumber),
       __func__,__FILE__,__LINE__);
    eC(ppr_set_min_size(context, minSize),
       __func__,__FILE__,__LINE__);
    eC(ppr_set_max_size(context, maxSize),
       __func__,__FILE__,__LINE__);
    eC(ppr_set_template_extraction_type(context,templateExtractor),
       __func__,__FILE__,__LINE__);
    
    eC(ppr_set_landmark_detector_type(context,detector,detectorMode),
       __func__,__FILE__,__LINE__);

    eC(ppr_initialize_context(context),
       __func__,__FILE__,__LINE__);
 
    eC(ppr_create_gallery(context,&pGallery),
       __func__,__FILE__,__LINE__);
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  }
  initialised=true;
}

/*!
 * Method loading galleries of images to internal structures of object
 * and creating ppr gallery for recognition
 *
 * \param galleries - object containing images that will be saved in this 
 * object
 *
 * \post Method recognise may be used safely now
 */

void PPRec::loadGalleries(Galleries& galleries){
  ppr_image_type pImg;
  ppr_object_list_type oList;
  ppr_template_type pTemplate;
  ppr_object_suitability_type recAble;
  std::stringstream sbuff;

  if(!initialised){
    Exception ex(PITTPATT_ERROR,"exception: using uninitialised object",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }
  try{  
    for(int i=0;i<galleries.totalSize();++i){
      for(int j=0;j<galleries.gallerySize(i);++j){
	Mat img=galleries.getPicture(i,j);
	Mat bw; //needed or OCV2.2 would segment fault

	if(img.channels()!=1){
	  Mat tmp;
	  cvtColor(img,tmp,CV_RGB2GRAY);
	  equalizeHist(tmp,bw);
	}else{
	  equalizeHist(img,bw);
	}

	eC(mat2PprImage(bw,pImg,PPR_RAW_IMAGE_GRAY8),
	   __func__,__FILE__,__LINE__);
	eC(ppr_detect_objects(context,pImg,&oList),
	   __func__,__FILE__,__LINE__);
	// cerr<<"Photo: "<<galleries.getGalleryLabel(i)<<'('<<j<<") ";
	// cerr<<"Objects found: "<<oList.num_objects;

	for(int k=0;k<oList.num_objects;++k){
	  int id;
	  char label[30];
	  //	  might be unnecessary
	  // eC(ppr_detect_landmarks_from_object(context,pImg,&oList.objects[k]),
	  //    __func__,__FILE__,__LINE__); 

	  eC(ppr_is_object_suitable_for_recognition(context,oList.objects[k],
						    &recAble),
	     __func__,__FILE__,__LINE__);
	  if(PPR_OBJECT_SUITABLE_FOR_RECOGNITION==recAble){
	    eC(ppr_extract_template_from_object(context,pImg,oList.objects[k],
						&pTemplate),
	       __func__,__FILE__,__LINE__);
	    sprintf(label,"%s %d",galleries.getGalleryLabel(i).c_str(),i);
	    eC(ppr_set_template_string(context,&pTemplate,label),
	       __func__,__FILE__,__LINE__);
	    eC(ppr_copy_template_to_gallery(context,&pGallery,pTemplate,&id),
	       __func__,__FILE__,__LINE__);
	    ppr_free_template(pTemplate);
	    //	    cerr<<" Template found";
	    idList.push_back(id);
	    lList.push_back(i);
	  }
	  ppr_free_image(pImg);
	}
	//	cerr<<endl;
      }
    }
    for(unsigned i=1;i<idList.size();++i){
      if(lList[i-1]!=lList[i]){
	eC(ppr_set_template_relationship(context,&pGallery,idList[i-1],idList[i],
					 PPR_RELATIONSHIP_DIFFERENT_SUBJECTS),
	   __func__,__FILE__,__LINE__);
      }else{
	eC(ppr_set_template_relationship(context,&pGallery,idList[i-1],idList[i],
					 PPR_RELATIONSHIP_SAME_SUBJECT),
	   __func__,__FILE__,__LINE__);
      }
    }
    eC(ppr_cluster_gallery(context,pGallery,0,&sList),
       __func__,__FILE__,__LINE__);
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  }
  loaded=true;
}


/*!
 * Method allowing to load previously computed galleries
 *
 * \param target - target to file containing prevoiusly saved ppr galleries 
 *
 * \pre target points to a file created with PPRec::savePrecomputedGalleries or
 * containig the informations contained by previously mentioned
 *
 * \post Method recognise may be used safely now
 */

void PPRec::loadPrecomputedGalleries(const string& target){
  if(!initialised){
    Exception ex(PITTPATT_ERROR,"exception: using uninitialised object",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }
  
  try{
    eC(ppr_read_gallery_with_subject_list(context,target.c_str(),&pGallery,
					  &sList),
	   __func__,__FILE__,__LINE__);
    string label;
    int iLabel;
    char cLabel[30];
    char sbuff[30];
    lList.clear();
    idList.clear();
    for(int i=0;i<sList.num_subjects;++i){
      eC(ppr_get_template_string_by_id(context,pGallery,
				       sList.subjects[i].
				       template_ids[0],cLabel),
	 	   __func__,__FILE__,__LINE__);
  
      sscanf(cLabel,"%s %d",sbuff,&iLabel);
      cerr<<sbuff<<" "<<iLabel<<endl;
      
      for(int j=0;j<sList.subjects[i].num_template_ids;++j){
	idList.push_back(sList.subjects[i].template_ids[j]);
	lList.push_back(iLabel);
      }
    }
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  }
  loaded=true;
}

/*!
 * Method allowing to save computed ppr gallery to file
 *
 * \param target - path and name of ppr gallery file
 *
 */
  
void PPRec::savePrecomputedGalleries(const string& target){
  if(!initialised){
    Exception ex(PITTPATT_ERROR,"exception: using uninitialised object",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }

  if(!loaded){
    Exception ex(PITTPATT_ERROR,"exception: no data loaded to object",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }

  try{
    eC(ppr_write_gallery_with_subject_list(context,target.c_str(),pGallery,
					   sList),
       	   __func__,__FILE__,__LINE__);
  }
    catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  }
}

/*!
 * This method does nothing, because all computing is done in method
 * load galleries
 */

void PPRec::compute(){
  //all is done in load gallery
}

/*!
 * Method meant to clear up the object. Right now does nothing
 */

void PPRec::clear(){

}

/*!
 * Method allows to use PPRec to classify pattern on image pointed by target
 * 
 * \param target - image to be classified
 *
 * \return Ranked list of Result objects, sorted descending by mean value
 *
 * \pre Image pointed by target has the same number of pixels as images from 
 * galleries used for computing ppr galleries. loadGalleries, or 
 * loadPrecomputedGalleries method was succcesfully used
 */

list<Result> PPRec::recognise(const string& path){
 
  if(!initialised){
    Exception ex(PITTPATT_ERROR,"exception: using uninitialised object",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }

  if(!loaded){
    Exception ex(PITTPATT_ERROR,"exception: no data loaded to object",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }

 try{
    Mat img;
    img=imread(path);
    return recognise(img);
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  }
}

/*!
 * Method allows to use PPRec to recognise data in img object
 *
 * \param img - object containing data to be classified
 *
 * \return Ranked list of Result objects, sorted descending by mean value
 *
 * \pre Img contains the same number of values as images from 
 * galleries used for computing ppr galleries. loadGalleries, or 
 * loadPrecomputedGalleries method was succcesfully used
 */


list<Result> PPRec::recognise(Mat &img){
  ppr_image_type pImg;
  
  ppr_object_list_type oList;
  ppr_template_type pTemplate;
  ppr_object_suitability_type recAble;
  ppr_score_list_type scList;
  ppr_gallery_type tGallery;
  ppr_similarity_matrix_type similarityMatrix;
  ppr_subject_list_type sTList;
  ppr_index_list_type iList;

  Result result;

  list<Result> results;
  
  Mat tmp,eq;
  
  if(!initialised){
    Exception ex(PITTPATT_ERROR,"exception: using uninitialised object",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }

  if(!loaded){
    Exception ex(PITTPATT_ERROR,"exception: no data loaded to object",
		 __func__,__FILE__,__LINE__);
    throw ex;
  }

  scList.num_scores=oList.num_objects=0;
  scList.scores=NULL;
  oList.objects=NULL;
  
  result.min=result.max=result.mean=0;
  result.label=-1;

  results.clear();
  //cerr<<"start"<<endl;
  try{
    if(img.channels()!=1){
      cvtColor(img,tmp,CV_RGB2GRAY);
    }else{
      tmp=img;
    }
    equalizeHist(tmp,eq);

    eC(mat2PprImage(eq,pImg,PPR_RAW_IMAGE_GRAY8),
       __func__,__FILE__,__LINE__);
    eC(ppr_detect_objects(context,pImg,&oList),
       __func__,__FILE__,__LINE__);

    //    cerr<<"Objects found: "<<oList.num_objects<<endl;
    

    eC(ppr_create_gallery(context,&tGallery),
       __func__,__FILE__,__LINE__); 
   
    if(oList.num_objects==1){
      //cerr<<"czy obiekt sie nadaje"<<endl;
      eC(ppr_is_object_suitable_for_recognition(context,oList.objects[0],
						&recAble),
	 __func__,__FILE__,__LINE__);
      if(PPR_OBJECT_SUITABLE_FOR_RECOGNITION==recAble){
	int id;
	//cerr<<"extrakcja template"<<endl;
	eC(ppr_extract_template_from_object(context,pImg,oList.objects[0],
					    &pTemplate),
	   __func__,__FILE__,__LINE__);
	//cerr<<"do temp galerii"<<endl;
	eC(ppr_copy_template_to_gallery(context,&tGallery,pTemplate,&id),
	   __func__,__FILE__,__LINE__);
	ppr_free_template(pTemplate);
	//cerr<<"cluster"<<endl;
	eC(ppr_cluster_gallery(context,tGallery,0,&sTList),
	   __func__,__FILE__,__LINE__);
	//cerr<<"compare"<<endl;
	eC(ppr_compare_galleries(context,tGallery,pGallery,&similarityMatrix),
	   __func__,__FILE__,__LINE__); 
	//cerr<<"get list"<<endl;
	eC(ppr_get_ranked_subject_list_for_subject(context,similarityMatrix,
						   sTList.subjects[0],sList,
						   1000,-100,&iList,&scList),
	   __func__,__FILE__,__LINE__); 
	
      }else{
	cerr<<"nie nadaje się"<<endl;
	ppr_free_gallery(tGallery);
	Exception ex(PITTPATT_ERROR,
		     "Exception: image not suitable for recognition",
		     __func__,__FILE__,__LINE__);
	throw ex;
      }
      //      cerr<<"przygotoanie rezultatów"<<endl;
      Result result;
      result.min=result.max=result.mean=0;
      result.label=-1;
      
      {
	char cLabel[30];
	char sBuff[30];
	int iLabel;
	//	cerr<<"petla"<<endl;
	for(int i=0;i<scList.num_scores;++i){
	  int index=iList.indices[i];
	  cerr<<scList.scores[i]<<endl;
	  //	  result.mean=scList.scores[index];
	  result.score=scList.scores[index];
	  cerr<<"getTemplateString"<<endl;
	  eC(ppr_get_template_string_by_id(context,pGallery,
					   sList.subjects[index].
					   template_ids[0],cLabel),
	     __func__,__FILE__,__LINE__);
	  
	  //cerr<<"przypisanie"<<endl;
	  sscanf(cLabel,"%s %d",sBuff,&iLabel);
	  //  cerr<<sBuff<<" "<<iLabel<<endl;
	  result.label=iLabel;
	  // cerr<<result.label<<" "<<result.mean<<endl;
	  //result.min=result.max=0;
	  results.push_back(result);
	}
	//cerr<<"popetla"<<endl;
      }
    }else{
      //cerr<<"za duzo obiektów"<<endl;
      ppr_free_object_list(oList);
      // ppr_free_score_list(scList);
      ppr_free_gallery(tGallery);
      // ppr_free_similarity_matrix(similarityMatrix);
      // ppr_free_subject_list(sTList);
      //      ppr_free_index_list(iList);
      ppr_free_image(pImg);

      Exception ex(PITTPATT_ERROR,
		   "Exception: image contains more than one target",
		   __func__,__FILE__,__LINE__);
      throw ex;
    }      
      
    //cerr<<"czyszczenie"<<endl;
    ppr_free_object_list(oList);
    ppr_free_score_list(scList);
    ppr_free_gallery(tGallery);
    ppr_free_similarity_matrix(similarityMatrix);
    ppr_free_subject_list(sTList);
    ppr_free_index_list(iList);
    ppr_free_image(pImg);
  }
    
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    throw ex;
  }
  cerr<<"zwrot "<< results.size() << " wyników"<<endl;
  return results;
}

/*!
 * Destructor. Cleans up objects created by PittPatt 
 */

PPRec::~PPRec(){
  //  cerr<<"destructor"<<endl;
  if(initialised){
    ppr_free_gallery(pGallery);
    if(loaded){
      ppr_free_subject_list(sList);
    }
    ppr_release_context(context);
    ppr_finalize_sdk();
  }
}
    
#endif
