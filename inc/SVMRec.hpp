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

  void readFromBinary(cv::Mat &data,const string& path,cv::Size size,int type);
  void writeToBinary(cv::Mat &data,const string& path);

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

