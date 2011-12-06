///\file
/// \brief Main function file for galleries precomputing program
///\author Michał Orynicz
#include <iostream>
#include "thread.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <list>
#include <cmath>
#include <string>
#include <sstream>
#include "filesystem.hpp"
#include "timer.hpp"
#include "Galleries.hpp"
#include "Rec.hpp"
#include "PCARec.hpp"
#include "SVMRec.hpp"
#include "FaceFactor.hpp"

#include "PittpattPresence.hpp"
#ifdef PITTPATT_PRESENT
#include "PPRec.hpp"

#define PPREC_PRECOMPUTED
#endif
#include <sstream> //do nazw plików

//#include "Lapacz.hpp"

#define PCAREC
//#define PCAREC_PRECOMPUTED
#define SVMREC
//#define SVMREC_PRECOMPUTED

using namespace cv;
using namespace std;

/*!
 * \brief Function printing how much time it took to finish a task
 *
 * \param time - timer to compute time elapsed
 */
void timeElapsed(const boost::timer &time){
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
      <<fmod(time.elapsed(),60) <<"s"<<endl;
}

int main(int argc,char **argv){

  string address;

  Galleries galleries;

  vector<Rec*> alg;

  if(argc != 2){
    cerr<<" Error: incorrect number of arguments. Correct invocation:"<<endl
	<<argv[0]<<" galleries_folder"<<endl;
    return 1;
  }

  address=argv[1];

  try{
    galleries.setPath(address);
    galleries.load("galeria.xml");
  }
  catch(Exception ex){
    cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;
    cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
  }
  
  cout<<"hi thar"<<endl;

  
  
  try{
    boost::timer time,algTime,totalTime;
    
    cout<<"Preparing precomputed gallleries"<<endl;
    totalTime.restart();
#ifdef PITTPATT_PRESENT
    alg.push_back(new PPRec);
    cout<<alg.back()->getName()<<" queued"<<endl;
#endif
#ifdef PCAREC
    alg.push_back(new PCARec);
    cout<<alg.back()->getName()<<" queued"<<endl;
#endif
#ifdef SVMREC
    alg.push_back(new SVMRec);
    cout<<alg.back()->getName()<<" queued"<<endl;
#endif
    
    for(int i=0;i<alg.size();++i){
      cout<<"Preparing precomputed galeries for "<<alg[i]->getName()<<"\n"<<endl;
      algTime.restart();
      cout<<"initialising"<<endl;
      time.restart();
      alg[i]->initialize();
      timeElapsed(time);
      cout<<endl;

      cout<<"loadnig galleries"<<endl;
      time.restart();
      alg[i]->loadGalleries(galleries);
      timeElapsed(time);
      cout<<endl;

      cout<<"computing"<<endl;
      time.restart();
      alg[i]->compute();
      timeElapsed(time);
      cout<<endl;

      cout<<"saving precomputed galleries"<<endl;
      if(!boost::filesystem::exists((alg[i]->getName()+"Data"))){
	boost::filesystem::create_directory((alg[i]->getName()+"Data"));
      }
      time.restart();
      alg[i]->savePrecomputedGalleries((alg[i]->getName()+"Data")
				       +"/"+alg[i]->getName()+".xml");
      timeElapsed(time);
      cout<<"Precomputed galleires for "<<alg[i]->getName()<<" done and saved"
				       <<endl;
      timeElapsed(algTime);
      cout<<endl;
    }
    cout<<"Precomputing galleries done"<<endl;
    timeElapsed(totalTime);
    
  }
  catch(Exception ex){
    cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
  }

 return 0;
}
