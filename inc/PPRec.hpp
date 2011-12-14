/// \file
/// \brief Header file for class PPRec
/// \author Michał Orynicz

#include "Rec.hpp"
#include <string>
#include "PittpattPresence.hpp"
#ifdef PITTPATT_PRESENT
#include <pittpatt/pittpatt_ftr_sdk.h>

#ifndef PP_REC_HPP
#define PP_REC_HPP

class PPRec:public Rec{
  string modelPath; ///< Path to PittPatt models
  // string galleryFile; ///
  ppr_precision_type precision; ///< Precision of object detector
  ppr_landmark_detector_type detector; ///< Type of object detector
  int detectorMode; ///< Mode in which object detector works
  int threadNumber; ///< Number of threads used
  int threadRecognitionNumber; ///< Number of threads used for recognition
  int searchPrunning; ///< Aggressivenes of prunning
  int minSize; ///< Min face size
  int maxSize; ///< Max face size
  float adaptiveMinSize; ///< Min adaptive face size
  float adaptiveMaxSize; ///< Max adaptive face size
  float detectionThreshold; ///< Setting of certainity of object detector
  
  ///\brief Constraint on how much face can be rotated horizontally and still 
  /// detected
  ppr_frontal_yaw_constraint_type yawConstraint;

  ///\brief Template extractor type
  ppr_template_extraction_type templateExtractor;
  ppr_context_type context; ///< PittPatt context

  ppr_gallery_type pGallery; ///< PittPatt template gallery
  std::vector<int> idList; ///< List of template Id's.
  std::vector<int> lList; ///< List of template labels.
  ppr_subject_list_type sList; ///< List of subjects
  bool initialised,loaded; ///< Flags 

  ///\brief Error Check method
  void eC(ppr_error_type err,std::string func,std::string file,int line);
public:
  PPRec();///< Constructor 

  ///\brief Method allowing to load galleries to object
  virtual void initialize();

  ///\brief Method allowing to load previously computed data to object
  virtual void loadGalleries(Galleries& galleries);

  ///\brief Method allowing to load previously computed data to object
  virtual void loadPrecomputedGalleries(const std::string& path);

  ///\brief Method allowing to save computed data for later use
  virtual void savePrecomputedGalleries(const std::string& path);

  ///\brief Method computes data from galleries for recognition 
  virtual void compute();

  ///\brief Method clears the object
  virtual void clear();

  ///\brief Method performing recognition
  std::list<Result> recognise(const std::string& path);

  ///\brief Method performing recognition
  std::list<Result> recognise(cv::Mat& img);

  ///\brief Method performing recognition
  virtual ~PPRec();
};

#endif 
#endif
