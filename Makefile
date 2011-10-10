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

#run: detektor
#	./detektor kaskady/haarcascade_frontalface_default.xml \
#	galerie

all: kreatorGalerii porownywacz  masGalKreator videoKreator

kreatorGalerii: src/*.cpp inc/*.hpp obj/CapToGal.o obj/Lapacz.o obj/Galleries.o
	$(CC) -o kreatorGalerii  ${LIBS} obj/CapToGal.o obj/Lapacz.o \
	obj/Galleries.o	

porownywacz: src/*.cpp inc/*.hpp obj/main.o obj/Lapacz.o obj/Galleries.o \
	obj/PCARec.o obj/SVMRec.o
	$(CC) -o porownywacz  ${LIBS} obj/main.o obj/Lapacz.o obj/Galleries.o \
	obj/PCARec.o obj/SVMRec.o

masGalKreator: src/*.cpp inc/*.hpp obj/VideoToGal.o obj/Lapacz.o obj/Galleries.o
	$(CC) -o masGalKreator  ${LIBS} obj/VideoToGal.o  \
	obj/Galleries.o	

videoKreator: obj/VideoCap.o obj/Lapacz.o
	$(CC) -o videoKreator ${LIBS} obj/VideoCap.o obj/Lapacz.o

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
	rm obj/* porownywacz galeriator

