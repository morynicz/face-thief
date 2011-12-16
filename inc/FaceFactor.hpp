/// \file
/// \brief File containing constants that define size of pictures in galleries
/// \author Michał Orynicz

#ifndef FACE_FACTOR_HPP
#define FACE_FACTOR_HPP
///\brief OUT_HEIGHT/OUT_WIDTH-1
const float FACE_FACTOR=0.3;

const int OUT_WIDTH=200; ///< Width of pictures in pixels
///\brief Height of pictures in pixels 
const int OUT_HEIGHT=OUT_WIDTH+FACE_FACTOR*OUT_WIDTH;

#endif
