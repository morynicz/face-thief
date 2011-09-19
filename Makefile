CFLAGS= -I inc \
	-I /usr/local/include/opencv2 \
	-I /usr/include/boost \
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

all: galeriator porownywacz

galeriator: src/*.cpp inc/*.hpp obj/CapToGal.o obj/Lapacz.o obj/Galleries.o
	$(CC) -o galeriator  ${CFLAGS} obj/CapToGal.o obj/Lapacz.o \
	obj/Galleries.o	

porownywacz: src/*.cpp inc/*.hpp obj/main.o obj/Lapacz.o obj/Galleries.o
	$(CC) -o porownywacz  ${CFLAGS} obj/main.o obj/Lapacz.o obj/Galleries.o

obj/CapToGal.o: src/CapToGal.cpp inc/*.hpp
	$(CC) -o obj/CapToGal.o src/CapToGal.cpp -c ${CFLAGS}	

obj/main.o: src/main.cpp inc/*.hpp
	$(CC) -o obj/main.o src/main.cpp -c ${CFLAGS}

obj/Lapacz.o: inc/Lapacz.hpp src/Lapacz.cpp
	$(CC) -o obj/Lapacz.o src/Lapacz.cpp -c ${CFLAGS}

obj/Galleries.o: inc/Galleries.hpp src/Galleries.cpp
	$(CC) -o obj/Galleries.o src/Galleries.cpp -c ${CFLAGS}

clean:
	rm obj/* detektor