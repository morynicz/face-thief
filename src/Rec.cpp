///\file
///\brief File containing implementation of functions connected with class Rec
///\author Michał Orynicz

#include "Rec.hpp"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;


/*!
 * Function allowing to save data of Mat object to a binary file
 *
 * \param data - object containig data to be saved
 * \param path - path and name of file to which the data should be saved
 */


void writeToBinary(Mat &data,const string& path){
  std::ofstream out(path.c_str(),std::ofstream::binary);
  int bytes;
  switch(data.depth()){
  case CV_8U:
  case CV_8S:
    bytes=1;
    break;
  case CV_16U:
  case CV_16S:
    bytes=2;
    break;
  case CV_32F:
  case CV_32S:
    bytes=4;
    break;
  case CV_64F:
    bytes=8;
    break;
  default:
     cv::Exception err(UNKNOWN_MAT_TYPE,
			"Unknown Mat type, cannot write",
			__func__,__FILE__,__LINE__);
      throw err;
  }
  out.write((const char*)data.data,data.rows*data.cols*bytes
	    /*CV_32F->4*CV_8U*/);
  out.close();
}


/*!
 * Function reading contents of binary file pointed by target, and writing it's 
 * contents to data, based on size and type
 *
 * \param data [out] - object to which contents of file will be written
 * \param target - target to file to be read
 * \param size - dimensions of matrix saved in binary file
 * \param type - type of matrix saved in binary file
 */

void readFromBinary(cv::Mat &data,const string& target,Size size,int type){
  
    std::ifstream in(target.c_str(),std::ifstream::binary);
    
    //check size of file
    in.seekg(0,std::ifstream::end);
    int length=in.tellg();
    in.seekg(0);
    //allocate
    char *buff=new char[length];
    in.read(buff,length);
    {
  
      Mat tmp(size,type,buff);
      data=tmp.clone();
    }    
    delete buff;
    in.close();
  
}
