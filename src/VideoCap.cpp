///\file
/// \brief Main function file for video recording program
///\author Michał Orynicz

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;


/// Program recording a video stream from camera
int main(int argc,char *argv[]){
  VideoCapture cap;
  VideoWriter writ;
  char cont='e';
  Mat obr;
  long counter=0;

  if(argc<3){
    cerr<<"Error: not enough parameters."<<endl<<argv[0]
	<<" Input_source_name output_target_name"<<endl;
    return 1;
  }
  cap.open(atoi(argv[1]));
   writ.open(argv[2],CV_FOURCC('D','I','V','X'),10,Size(640,480));
  namedWindow("input",CV_WINDOW_NORMAL);
  do{
    ++counter;
    try{
      cap>>obr;
      writ<<obr;
      cerr<<counter<<endl;
      imshow("input",obr);
      cont=waitKey(100);
    }
    catch(Exception ex){
      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in fucntion "<<__func__;
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
    }
 }while(cont!='q');

  return 0;
}
