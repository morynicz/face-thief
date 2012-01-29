add_library(Recognitors STATIC Rec.cpp SVMRec.cpp PCARec.cpp)
add_library(Galleries STATIC Galleries.cpp)
add_library(Capture STATIC Catcher.cpp)
include_directories("../inc")