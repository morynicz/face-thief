#include "Galleries.hpp"
#include "opencv2/core/core.hpp"
#include <string>
#include <list>

#ifndef __REC__
#define __REC__

///Structure with results
struct Result{
  int label;
  double score;
};

///
class Rec{
protected:
  Galleries _gal;
public:
  Rec(){}
  virtual  void loadGalleries(Galleries& galleries)=0;
  virtual  void loadPrecomputedGalleries(const std::string& path){}
  virtual  void savePrecomputedGalleries(const std::string& path){}
  virtual  void compute()=0;
  virtual  void clear(){}
  virtual  std::list<Result> recognise(const std::string& path)=0;
  virtual  std::list<Result> recognise(cv::Mat& face)=0;
  virtual  ~Rec(){}
};

#endif
