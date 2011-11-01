#include "Rec.hpp"
#include <string>
#include "PittpattPresence.hpp"
#ifdef PITTPATT_PRESENT
#include <pittpatt/pittpatt_ftr_sdk.h>

#ifndef PP_REC_HPP
#define PP_REC_HPP

class PPRec:public Rec{
  string modelPath;
  string galleryFile;
  ppr_precision_type precision;
  ppr_landmark_detector_type detector;
  int detectorMode;
  int threadNumber;
  int threadRecognitionNumber;
  int searchPrunning;
  int minSize;
  int maxSize;
  float adaptiveMinSize;
  float adaptiveMaxSize;
  float detectionThreshold;
  ppr_frontal_yaw_constraint_type yawConstraint;
  ppr_template_extraction_type templateExtractor;
  ppr_context_type context;

  ppr_gallery_type pGallery;
  std::vector<int> idList;
  std::vector<int> lList;
  ppr_subject_list_type sList;
  void eC(ppr_error_type err,std::string func,std::string file,int line);
public:
  PPRec();
  virtual void initialise();
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
#endif
