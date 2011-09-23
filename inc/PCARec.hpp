#include "Rec.hpp"

#ifndef __PCA_REC__
#define __PCA_REC__

class PCARec:public Rec{
  cv::Mat _data;
  cv::Mat _vectors;
  cv::PCA _pca;
  std::list<int> _labelNr;
  cv::Mat _icovar;
public:
  PCARec();
  virtual void loadGalleries(Galleries& galleries);
  virtual void loadPrecomputedGalleries(const std::string& path);
  virtual void savePrecomputedGalleries(const std::string& path);
  virtual void compute();
  virtual void clear();
  std::list<Result> recognise(const std::string& path);
  std::list<Result> recognise(cv::Mat& img);
  virtual ~PCARec();
};

#endif 
  
