#include "Galleries.hpp"
#include "opencv2/core/core.hpp"
#include <string>

#ifndef __REC__
#define __REC__

///Structure with results
struct Result{
  std::string label;
  double score;
}

///
class Rec{
protected:
  Galleries _gal;
public:
  Rec();
  void loadGalleries(Galleries& galleries);
  void loadInternalGalleries(const std::string& path);
  void saveInternalGalleries(const std::string& path);
  void compute();
  void clear();
  std::list<Result> recognise(const std::string& path);
  std::list<Result> recognise(cv::Mat& face);
  ~Rec();
}

#endif
