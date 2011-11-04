#include "Galleries.hpp"
#include "opencv2/core/core.hpp"
#include <string>
#include <list>

#ifndef REC_HPP
#define REC_HPP

///Structure with results
struct Result{
  int label;
  double mean;
  double min;
  double max;
};

inline bool compareMeanResults(const Result &a,const Result &b){
  return a.mean>b.mean;
}

inline bool compareMinResults(const Result &a,const Result &b){
  return a.min < b.min;
}

///
class Rec{
protected:
  Galleries _gal;
  std::string name;
public:
  Rec(){}
  virtual void initialise(){};
  virtual void loadGalleries(Galleries& galleries)=0;
  virtual void loadPrecomputedGalleries(const std::string& path){}
  virtual void savePrecomputedGalleries(const std::string& path){}
  virtual void compute()=0;
  virtual void clear(){}
  virtual std::list<Result> recognise(const std::string& path)=0;
  virtual std::list<Result> recognise(cv::Mat& face)=0;
  string getName()const{return name;}
  virtual ~Rec(){}
};

#endif
