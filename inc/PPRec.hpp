#include "Rec.hpp"
#include <string>
#include "pittpatt_ftr_sdk.h"

#ifndef __PP_REC__
#define __PP_REC__

class PPRec:public Rec{
  string modelPath;
  string galleryFile;
  ppr_precision_type precision;
  ppr_landmark_detector_type detektor;
  int threadNumber;
  int threadRecognitionNumber;
  int searchPrunning;
  int minSize;
  int maxSize;
  float adaptiveMinSize;
  float adaptiveMaxSize;
  float detectionThreshold;
  ppr_frontal_jaw_constraint_type yawConstraint;
  ppr_template_extraction_type templateExtractor;
  ppr_context_type context;

  ppr_gallery_type pGallery;

public:
  PPRec();
  virtual void loadGalleries(Galleries& galleries);
  virtual void loadPrecomputedGalleries(const std::string& path);
  virtual void savePrecomputedGalleries(const std::string& path);
  virtual void compute();
  virtual void clear();
  //void set
  std::list<Result> recognise(const std::string& path);
  std::list<Result> recognise(cv::Mat& img);
  virtual ~PPRec();
};

#endif 
