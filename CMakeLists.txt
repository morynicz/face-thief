cmake_minimum_required(VERSION 2.6)

project(FACE_REC)

# Version number
set(FACE_RECOGNITION_VERSION_MAJOR 2)

set(FACE_RECOGNITION_VERSION_MINOR 0)

set(OPENCV_LIBRARIES ${OPENCV_LIBRARIES} opencv_core
opencv_ml opencv_highgui opencv_imgproc opencv_objdetect)

set(BOOST_LIBRARIES ${BOOST_LIBRARIES} boost_thread  boost_filesystem boost_system)

add_definitions(-Wall -pedantic -g)

add_subdirectory(src)
add_subdirectory(doc)

find_package(OpenCV REQUIRED)

find_package(Boost REQUIRED thread filesystem system)



include_directories("${FACE_REC_SOURCE_DIR}/inc")

add_executable(computer src/Computer.cpp)
add_executable(comparator src/Comparator.cpp)
add_executable(singleGalCreator src/CapToGal.cpp)
add_executable(serialGalCreator src/VideoToGal.cpp)
add_executable(detector src/Detector.cpp)
add_executable(videoCreator src/VideoCap.cpp)
add_executable(tester src/Tester.cpp)

target_link_libraries(computer Recognitors Galleries ${BOOST_LIBRARIES} ${OPENCV_LIBRARIES})

target_link_libraries(comparator Recognitors Galleries ${BOOST_LIBRARIES} ${OPENCV_LIBRARIES})

target_link_libraries(singleGalCreator Capture Recognitors Galleries ${BOOST_LIBRARIES} ${OPENCV_LIBRARIES})

target_link_libraries(detector Capture Recognitors Galleries ${BOOST_LIBRARIES} ${OPENCV_LIBRARIES})

target_link_libraries(videoCreator Capture Galleries ${BOOST_LIBRARIES} ${OPENCV_LIBRARIES})

target_link_libraries(serialGalCreator Recognitors Galleries ${BOOST_LIBRARIES} ${OPENCV_LIBRARIES}) 

target_link_libraries(singleGalCreator Recognitors Galleries ${BOOST_LIBRARIES} ${OPENCV_LIBRARIES})

target_link_libraries(tester Recognitors Galleries ${BOOST_LIBRARIES} ${OPENCV_LIBRARIES})

