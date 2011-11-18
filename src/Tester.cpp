#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <list>
#include <cmath>
#include <string>
#include <sstream>
#include "filesystem.hpp"
#include "Galleries.hpp"
#include "PCARec.hpp"
#include "SVMRec.hpp"
#include "timer.hpp"
#include "FaceFactor.hpp"
#include "PittpattPresence.hpp"
#ifdef PITTPATT_PRESENT
#include "PPRec.hpp"

#define PPREC_PRECOMPUTED

#endif
#include <sstream> //do nazw plików
#include <algorithm>



//#define PCAREC
//#define PCAREC_PRECOMPUTED
//#define SVMREC
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


  Mat obr,eq;
  Mat mid;
  Mat bw;
  Mat gemben;

  string zdjecie;

  Size rozm(OUT_WIDTH,OUT_HEIGHT);
  
  string trainAddress;
  string validAddress;

  Galleries mainGalleries;
  Galleries trainGalleries;
  Galleries validGalleries;

  vector<Rect> twarze; 
  
  vector<string> galAddresses;

  int noOfSubsets;

 
  if(argc!=3){
    cerr<<"Error: incorrect number of arguments. Correct invocation:"<<endl
	<<argv[0]<<" galleries_folder no_of_subsets"<<endl;
    return 1;
  }

  try{
    noOfSubsets=atoi(argv[2]);
    //noOfSubsets=3;
    try{
      trainGalleries.setPath(argv[1]);
      trainGalleries.load("galeria.xml");
      trainGalleries.createKSubsets(noOfSubsets,"sub",galAddresses);
    }
    catch(Exception ex){
      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	  <<" in function "<<__func__<<endl;
      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
      throw ex;	  
    }
    cout<<"hi thar"<<endl;


    boost::timer time;
    boost::timer totalTime;
    boost::timer lapTime;
    // vector<bool> selector;
    // selector.resize(noOfSubsets);


    vector<double> score;
    {
      int noOfAlgs=0;
#ifdef PITTPATT_PRESENT
      ++noOfAlgs;
#endif
     
#ifdef SVMREC
      ++noOfAlgs;
#endif

#ifdef PCAREC
      ++noOfAlgs;
#endif

      score.resize(noOfAlgs,0);
    }

    for(int i=0;i<noOfSubsets;++i){
      cout<<"subset "<<i+1<<" of "<<noOfSubsets<<endl;
      lapTime.restart();
      vector<double> ptScore;
      ptScore.resize(score.size(),0);
      Galleries validation;
      Galleries training;
      validation.setPath(argv[1]);
      training.setPath(argv[1]);
      for(int j=0;j<noOfSubsets;++j){
	if(i==j){
	  validation.load(galAddresses[j]);
	}else{
	  training.load(galAddresses[j]);
	}
      }

      vector<Rec*> alg;

 
#ifdef PITTPATT_PRESENT
      alg.push_back(new PPRec);
#endif

#ifdef SVMREC
      alg.push_back(new SVMRec);
#endif
     
#ifdef PCAREC
      alg.push_back(new PCARec);
#endif

      try{
	for(int z=0;z<alg.size();++z){
	  cout<<alg[z]->getName()<<" initialisng"<<endl;
	  time.restart();
	  alg[z]->initialise();
	  cout<<alg[z]->getName()<<" initialised"<<endl;
	  timeElapsed(time);
	  // cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	  //     <<fmod(time.elapsed(),60) <<"s"<<endl;
	  cout<<alg[z]->getName()<<" galleries loading"<<endl;
	  time.restart();
	  alg[z]->loadGalleries(training);
	  cout<<alg[z]->getName()<<" galleries loaded"<<endl;
	  timeElapsed(time);
	  // cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	  //     <<fmod(time.elapsed(),60) <<"s"<<endl;

	  cout<<alg[z]->getName()<<" computing"<<endl;
	  time.restart();
	  alg[z]->compute();
	  cout<<alg[z]->getName()<<" computed"<<endl;
      	  timeElapsed(time);
	  // cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	  //     <<fmod(time.elapsed(),60) <<"s"<<endl;
	  time.restart();
	  cout<<endl;
	}
      }
      catch(Exception ex){
	cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	    <<" in function "<<__func__<<endl;
	cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
	throw ex;
      }

      try{
	int numberOfPhotos=0;
	for(int j=0;j<validation.totalSize();++j){
	  numberOfPhotos+=validation.gallerySize(j);
	}
	double photosDone=0;

	for(int j=0;j<validation.totalSize();++j){
	  string label=validation.getGalleryLabel(j);
	  for(int k=0;k<validation.gallerySize(j);++k){
	    Mat image=validation.getPicture(j,k);
	    cout<<"on photo: "<<label<<endl;
	    try{
	      string bestMatch;
	      for(int z=0;z<alg.size();++z){
		try{
		  cout<<alg[z]->getName()<<" recognising"<<endl;
		  time.restart();
		  Mat tmp=eq.clone();
		  std::list<Result> wyniki=alg[z]->recognise(image);
		  cout<<alg[z]->getName()<<" recognised "<<endl;
		  for(std::list<Result>::iterator sit=wyniki.begin();
		      sit!=wyniki.end();++sit){
		  
		    cout<<training.getGalleryLabel(sit->label)<<" "<<sit->mean
			<<" "<<sit->max<<" "<<sit->min<<endl;
		  }
		
		  if(!wyniki.empty()){
		    bestMatch=training.getGalleryLabel(wyniki.front().label);
		    cerr<<endl<<alg[z]->getName()+" "+bestMatch
			<<endl<<endl;
		    if(bestMatch==label){
		      ptScore[z]+=1;
		      cout<<"trafienie"<<endl;
		    }else{
		      cout<<"pudło"<<endl;
		    }
		  }	
		  timeElapsed(time);
		  // cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
		  //     <<fmod(time.elapsed(),60) <<"s"<<endl;
		  cout<<endl;
		  cout<<"\t\t"
		      <<((double)photosDone)/((double)numberOfPhotos)*100
		      <<"% of this lap"<<endl<<endl;
		}
		catch(Exception ex){
		  cerr<<ex.code<<" "<<ex.err<<endl
		      <<ex.func<<endl<<ex.line<<endl;
		}
	      }
	    }
	    catch(Exception ex){
	      cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
		  <<" in function "<<__func__<<endl;
	      cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
	      throw ex;
	    }
	  }
	}
	for(int k=0;k<ptScore.size();++k){
	  ptScore[k]/=numberOfPhotos;
	  cout<<alg[k]->getName()<<" ptScore: "<<ptScore[k]<<endl;
	  score[k]+=ptScore[k];
	  cout<<alg[k]->getName()<<" Score: "<<score[k]<<endl;
	  cout<<((double) i)/((double)noOfSubsets)*100<<"% done"<<endl<<endl;
	}
      }
      catch(Exception ex){
	cerr<<"Exception passed up through "<<__FILE__<<':'<<__LINE__
	    <<" in function "<<__func__<<endl;
	cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
	throw ex;
      }
      //      cerr<<"deletion"<<endl;
      for(int k=0;k<alg.size();++k){
	delete alg[k];
      }
      //      cerr<<"deleted"<<endl;
      cout<<"subset "<<i+1<<" of "<<noOfSubsets<<" validated"<<endl;
      timeElapsed(lapTime);
    }
    cout<<"all subsets validated"<<endl;
    timeElapsed(totalTime);

    {
      vector<Rec*> alg;
     
#ifdef PITTPATT_PRESENT
      alg.push_back(new PPRec);
#endif
     
#ifdef PCAREC
      alg.push_back(new PCARec);
#endif
#ifdef SVMREC
      alg.push_back(new SVMRec);
#endif
      for(int j=0;j<score.size();++j){
	score[j]/=noOfSubsets;
	cout<<alg[j]->getName()<<" Recognition score: "<<score[j]*100<<"%"<<endl;
      }
      //  cerr<<"second deletion"<<endl;
      for(int k=0;k<alg.size();++k){
	delete alg[k];
      }
    }
  }
  catch(Exception ex){
    cerr<<"Exception caught in "<<__FILE__<<':'<<__LINE__
	<<" in function "<<__func__<<endl;;
    cerr<<ex.code<<endl<<ex.err<<endl<<ex.func<<endl<<ex.line<<endl;
  }
  //  cerr<<"before return"<<endl;

  return 0;
}
/*
  #ifdef PITTPATT_PRESENT
  cout<<"PPR initialisng"<<endl;
  time.restart();
    
  alg.push_back(new PPRec);
  alg.back()->initialise();
  cout<<"PPR initialised"<<endl;
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
  <<fmod(time.elapsed(),60) <<"s"<<endl;
  #ifndef PPREC_PRECOMPUTED
  cout<<"PPR galleries loading"<<endl;
  time.restart();
  alg.back()->loadGalleries(galleries);
  cout<<"PPR galleries loaded"<<endl;
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
  <<fmod(time.elapsed(),60) <<"s"<<endl;

  cout<<"PPR computing"<<endl;
  time.restart();
  alg.back()->compute();
  cout<<"PPR computed"<<endl;
      
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
  <<fmod(time.elapsed(),60) <<"s"<<endl;
  time.restart();
  cout<<"PPR saving precomputed galleries"<<endl;
  alg.back()->savePrecomputedGalleries((alg.back()->getName()+"Data")
  +"/"+alg.back()->getName()+".xml");
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
  <<fmod(time.elapsed(),60) <<"s"<<endl;

  #elseif
  cout<<"PPR loading precomputed galleries"<<endl;
  time.restart();
  alg.back()->loadPrecomputedGalleries((alg.back()->getName()+"Data")
  +"/"+alg.back()->getName()+".xml");
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
  <<fmod(time.elapsed(),60) <<"s"<<endl;
  #endif   

  #endif
 
  #ifdef PCAREC
  cout<<"Starting PCA"<<endl;
  alg.push_back(new PCARec);
  alg.back()->initialise();
  cout<<"finished in "<<time.elapsed()<<"s"<<endl;
  #ifndef PCAREC_PRECOMPUTED

  cout<<"PCA: Loading galleries"<<endl;
  alg.back()->loadGalleries(galleries);
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
  <<fmod(time.elapsed(),60) <<"s"<<endl;
     
  cout<<"PCA: Computing"<<endl;
  time.restart();
  alg.back()->compute();
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
  <<fmod(time.elapsed(),60) <<"s"<<endl;


  cout<<"PCA: saving"<<endl;
  time.restart();
  alg.back()->savePrecomputedGalleries((alg.back()->getName()+"Data")
  +"/"+alg.back()->getName()+".xml");
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
  <<fmod(time.elapsed(),60) <<"s"<<endl;
  #else
  cout<<"PCA: Loading precomputed galleries"<<endl;
  time.restart();
  alg.back()->loadPrecomputedGalleries((alg.back()->getName()+"Data")
  +"/"+alg.back()->getName()+".xml");
  cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
  <<fmod(time.elapsed(),60) <<"s"<<endl;

#endif
#endif
   
#ifdef SVMREC
    cerr<<"SVM starting"<<endl;
    time.restart();
    alg.push_back(new SVMRec);
    alg.back()->initialise();
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;

#ifndef SVMREC_PRECOMPUTED
    cerr<<"SVM: loading galleries"<<endl;
    time.restart();
    alg.back()->loadGalleries(galleries);
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
    
    cerr<<"SVM: computing"<<endl;
    time.restart();
    alg.back()->compute();
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;

    cout<<"SVM: saving"<<endl;
    time.restart();
    alg.back()->savePrecomputedGalleries((alg.back()->getName()+"Data")
				       +"/"+alg.back()->getName()+".xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
#else    
    cout<<"SVM: Loading precomputed galleries"<<endl;
    time.restart();
    alg.back()->loadPrecomputedGalleries((alg.back()->getName()+"Data")
				       +"/"+alg.back()->getName()+".xml");
    cout<<"finished in "<<floor(time.elapsed()/60)<<"min "
	<<fmod(time.elapsed(),60) <<"s"<<endl;
#endif
#endif
    */
