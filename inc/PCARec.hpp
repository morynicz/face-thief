#include "Rec.hpp"

#ifndef PCA_REC_HPP
#define PCA_REC_HPP

class PCARec:public Rec{
  cv::Mat _data;


  std::list<int> _labelNr;
  cv::Mat _icovar;

  static std::string DATA;
  static std::string VECTORS;
  static std::string ICOVAR;
  static std::string LABEL_NR;
  static std::string EIGENVALUES;
  static std::string EIGENVECTORS;
  static std::string MEAN;

public:
  cv::PCA _pca;
    cv::Mat _vectors;
  PCARec();
  virtual void loadGalleries(Galleries& galleries);
  virtual void loadPrecomputedGalleries(const std::string& path);
  virtual void savePrecomputedGalleries(const std::string& path);
  virtual void compute();
  virtual void clear();
  virtual std::list<Result> recognise(const std::string& path);
  virtual std::list<Result> recognise(cv::Mat& img);
  virtual ~PCARec();
};

#endif 
  
