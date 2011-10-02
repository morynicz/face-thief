#include "Rec.hpp"

#ifndef __LDA_REC__
#define __LDA_REC__

class LDARec:public Rec{
  

public:
  LDARec();
  virtual void loadGalleries(Galleries& galleries);
  virtual void loadPrecomputedGalleries(const std::string& path);
  virtual void savePrecomputedGalleries(const std::string& path);
  virtual void compute();
  virtual void clear();
  std::list<Result> recognise(const std::string& path);
  std::list<Result> recognise(cv::Mat& img);
  virtual ~LDARec();
};

#endif 
