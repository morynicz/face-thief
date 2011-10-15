#include "opencv2/ml/ml.hpp"
#include "Rec.hpp"
#include <string>
#include <list>
#include <vector>

#ifndef SVM_REC_HPP
#define SVM_REC_HPP

class SVMRec:public Rec{
  cv::Mat _data;
  cv::Mat _vectors;
  cv::PCA _pca;
  std::list<CvSVM> _svms;
  std::list<int> _labelNr;
  //cv::Mat _icovar;
  
  // static std::string DATA;
  static std::string VECTORS;
  // static std::string ICOVAR;
  static std::string LABEL_NR;
  static std::string EIGENVALUES;
  static std::string EIGENVECTORS;
  static std::string MEAN;
  static int POSITIVE;
  static int NEGATIVE;

public:
  SVMRec();
  virtual void loadGalleries(Galleries& galleries);
  virtual void loadPrecomputedGalleries(const std::string& path);
  virtual void savePrecomputedGalleries(const std::string& path);
  virtual void compute();
  virtual void clear();
  virtual std::list<Result> recognise(const std::string& path);
  virtual std::list<Result> recognise(cv::Mat& img);
  virtual ~SVMRec();
};

#endif 

