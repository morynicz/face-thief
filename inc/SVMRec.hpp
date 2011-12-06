/// \file
/// \brief Header file for class SVMRec
/// \author Michał Orynicz
#include "opencv2/ml/ml.hpp"
#include "Rec.hpp"
#include <string>
#include <list>
#include <vector>

#ifndef SVM_REC_HPP
#define SVM_REC_HPP

///\brief Class providing object recognition using support vector machines and PCA
class SVMRec:public Rec{
  cv::Mat _data; ///< Matrix containing pictures from loaded galleries
  cv::Mat _vectors; ///< matrix containing pictures projected to PCA space
  cv::PCA _pca; ///< object containing PCA
  std::list<CvSVM> _svms; ///< vector containing support vector machines
  std::list<int> _labelNr; ///< list of numeric labels of pictures
  ///\cond  
  static int POSITIVE;
  static int NEGATIVE;

  static std::string DATA_ROWS;
  static std::string DATA_COLS;
  static std::string DATA_TYPE;

  static std::string VECTORS;
  static std::string VEC_ROWS;
  static std::string VEC_COLS;
  static std::string VEC_TYPE;

  static std::string LABEL_NR;

  static std::string EIGENVALUES;
  static std::string EIGENVECTORS;
  static std::string EIGEN_ROWS;
  static std::string EIGEN_COLS;
  static std::string EIGEN_TYPE;

  static std::string MEAN;
  static std::string MEAN_COLS;
  static std::string MEAN_TYPE;

  static std::string SVMS_QUANTITY;
  static std::string SVMS;
  static std::string SVM;
  ///\endcond
public:
  SVMRec();
  virtual void loadGalleries(Galleries& galleries);
  virtual void loadPrecomputedGalleries(const std::string& target);
  virtual void savePrecomputedGalleries(const std::string& target);
  virtual void compute();
  virtual void clear();
  virtual std::list<Result> recognise(const std::string& target);
  virtual std::list<Result> recognise(cv::Mat& img);
  virtual ~SVMRec();
};

#endif 

