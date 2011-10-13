#include "PPRec.hpp"


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
  //templateExtractor=
    

  context=ppr_get_context();
  TRY(ppr_enable_tracking(context));
  TRY(ppr_enable_recognition(context));
  TRY
    
