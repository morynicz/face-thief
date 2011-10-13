INCLUDES= -Iinc \
	-I /usr/local/include/opencv2 \
	-I /usr/include/boost \
	-I ../../pittpatt/pittpatt_ftr_sdk-4.2.2-linux-x86/include 
LIBS=-lopencv_core \
	-lopencv_highgui \
	-lopencv_ml \
	-lopencv_objdetect \
	-lopencv_gpu  \
	-lboost_thread \
	-lboost_filesystem 
CFLAGS=${INCLUDES} -g -O0 -Wall -pedantic

CC=g++

S_GAL=kreator_galerii
M_GAL=masGalKreator
POR=porownywacz
VID=videoKreator

#run: detektor
#	./detektor kaskady/haarcascade_frontalface_default.xml \
#	galerie

all: kreatorGalerii porownywacz  masGalKreator videoKreator

${S_GAL}: src/*.cpp inc/*.hpp obj/CapToGal.o obj/Lapacz.o obj/Galleries.o
	$(CC) -o ${S_GAL}  ${LIBS} obj/CapToGal.o obj/Lapacz.o \
	obj/Galleries.o	

${POR}: src/*.cpp inc/*.hpp obj/main.o obj/Lapacz.o obj/Galleries.o \
	obj/PCARec.o obj/SVMRec.o
	$(CC) -o ${POR}  ${LIBS} obj/main.o obj/Lapacz.o obj/Galleries.o \
	obj/PCARec.o obj/SVMRec.o

${M_GAL}: src/*.cpp inc/*.hpp obj/VideoToGal.o obj/Lapacz.o obj/Galleries.o
	$(CC) -o ${M_GAL}  ${LIBS} obj/VideoToGal.o  \
	obj/Galleries.o	

${VID}: obj/VideoCap.o obj/Lapacz.o
	$(CC) -o ${VID} ${LIBS} obj/VideoCap.o obj/Lapacz.o

obj/CapToGal.o: src/CapToGal.cpp inc/*.hpp
	$(CC) -o obj/CapToGal.o src/CapToGal.cpp -c ${CFLAGS}	

obj/VideoToGal.o: src/VideoToGal.cpp inc/*.hpp
	$(CC) -o obj/VideoToGal.o src/VideoToGal.cpp -c ${CFLAGS}	


obj/main.o: src/main.cpp inc/*.hpp
	$(CC) -o obj/main.o src/main.cpp -c ${CFLAGS}

obj/Lapacz.o: inc/Lapacz.hpp src/Lapacz.cpp
	$(CC) -o obj/Lapacz.o src/Lapacz.cpp -c ${CFLAGS}

obj/Galleries.o: inc/Galleries.hpp src/Galleries.cpp
	$(CC) -o obj/Galleries.o src/Galleries.cpp -c ${CFLAGS}

obj/ocv2pit.o: inc/ocv2pit.hpp src/ocv2pit.cpp
	$(CC) -o obj/ocv2pit.o src/ocv2pit.cpp -c ${CFLAGS}

obj/PCARec.o: inc/PCARec.hpp inc/Rec.hpp src/PCARec.cpp
	$(CC) -o obj/PCARec.o src/PCARec.cpp -c ${CFLAGS}

obj/SVMRec.o: inc/Rec.hpp inc/SVMRec.hpp src/SVMRec.cpp
	$(CC) -o obj/SVMRec.o src/SVMRec.cpp -c ${CFLAGS}

obj/VideoCap.o: inc/Lapacz.hpp  src/VideoCap.cpp
	$(CC) -o obj/VideoCap.o src/VideoCap.cpp -c ${CFLAGS}

clean:
	rm obj/* ${VID} ${M_GAL} ${S_GAL} ${POR}

