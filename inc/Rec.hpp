/// \file
/// \brief Header file for class Rec
/// \author Michał Orynicz
#include "Galleries.hpp"
#include "opencv2/core/core.hpp"
#include <string>
#include <list>

#ifndef REC_HPP
#define REC_HPP

///Structure with results
struct Result{
  int label; ///< gallery label
  /* double mean; 
  double min;
  double max;*/
  double score; ///< score for gallery with asociated label
};
/// Function returning if a is greater than b 

/*!
 * \param a - first component
 * \param b- second component
 *
 * return a.score>b.score 
*/

inline bool compareDescending(const Result &a,const Result &b){
  return a.score>b.score;
}

/// Function returning if a is less than b 

/*!
 * \param a - first component
 * \param b- second component
 *
 * return a.score>b.score 
*/

inline bool compareAscending(const Result &a,const Result &b){
  return a.score < b.score;
}

/// \brief Base class for face recognition algorithms
class Rec{
protected:
  std::string name; ///< Name of algorithm
public:
  ///\brief Empty constructor
  Rec(){}; 

  ///\brief Method preparing object to work
  virtual void initialize(){}; 

  /// \brief Method loading galleries to object
  virtual void loadGalleries(Galleries& galleries)=0;
  /// \brief Method loading precomputed data
  virtual void loadPrecomputedGalleries(const std::string& target){}
  /// \brief Method saving computed data for further use
  virtual void savePrecomputedGalleries(const std::string& target){}
  /// \brief Method executing algorithm computations
  virtual void compute()=0;
  /// \brief Method clearing the object to initialized-state
  virtual void clear(){}
  /// \brief Method executing recognition of object on image
  virtual std::list<Result> recognise(const std::string& target)=0;
  /// \brief Method executing recognition of object on image
  virtual std::list<Result> recognise(cv::Mat& face)=0;
  /// \brief Method returning algorithm name
  string getName()const{return name;}
  ///\brief Destructor
  virtual ~Rec(){}
};

/// \brief Function for reading cv::Mat files from binary file
void readFromBinary(cv::Mat &data,const string& path,cv::Size size,int type);
/// \brief Function for saving cv::Mat files to binary file
void writeToBinary(cv::Mat &data,const string& path);

#endif
