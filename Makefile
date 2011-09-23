CFLAGS= -I inc \
	-I /usr/local/include/opencv2 \
	-I /usr/include/boost \
	-I ../../pittpatt/pittpatt_ftr_sdk-4.2.2-linux-x86/include \
	-lopencv_core \
	-lopencv_highgui \
	-lopencv_ml \
	-lopencv_objdetect \
	-lopencv_gpu  \
	-lboost_thread \
	-lboost_filesystem \
	-g -O0

CC=g++

#run: detektor
#	./detektor kaskady/haarcascade_frontalface_default.xml \
#	galerie

all: galeriator porownywacz obj/PCARec.o

galeriator: src/*.cpp inc/*.hpp obj/CapToGal.o obj/Lapacz.o obj/Galleries.o
	$(CC) -o galeriator  ${CFLAGS} obj/CapToGal.o obj/Lapacz.o \
	obj/Galleries.o	

porownywacz: src/*.cpp inc/*.hpp obj/main.o obj/Lapacz.o obj/Galleries.o
	$(CC) -o porownywacz  ${CFLAGS} obj/main.o obj/Lapacz.o obj/Galleries.o \
	obj/PCARec.o

obj/CapToGal.o: src/CapToGal.cpp inc/*.hpp
	$(CC) -o obj/CapToGal.o src/CapToGal.cpp -c ${CFLAGS}	

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

clean:
	rm obj/* detektor