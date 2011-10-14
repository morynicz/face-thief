PITPATT_HOST="rab"
ifeq (${HOSTNAME}, ${PITTPATT_HOST})
INCLUDES= -Iinc \
	-I /usr/local/include/opencv2 \
	-I /usr/include/boost \
	-I ../../pittpatt/pittpatt_sdk/source/utilities \
	-I ../../pittpatt/pittpatt_sdk/include 
else
INCLUDES= -Iinc \
	-I /usr/local/include/opencv2 \
	-I /usr/include/boost 
endif

LIBS=-lopencv_core \
	-lopencv_highgui \
	-lopencv_ml \
	-lopencv_objdetect \
	-lopencv_gpu  \
	-lboost_thread \
	-lboost_filesystem 

#-L ../../pittpatt/pittpatt_sdk/ 
PP_LIBS= -lpittpatt_ftr_sdk \
	-lpittpatt_raw_image \
	-lpittpatt_recognition_core  

CFLAGS=${INCLUDES} -g -O0 -Wall -pedantic

CC=g++

S_GAL=kreatorGalerii
M_GAL=masGalKreator
POR=porownywacz
VID=videoKreator

S_GAL_OBJ=obj/CapToGal.o obj/Lapacz.o obj/Galleries.o
M_GAL_OBJ= obj/VideoToGal.o obj/Lapacz.o obj/Galleries.o

ifeq (${HOSTNAME}, ${PITTPATT_HOST})
POR_OBJ=obj/main.o obj/Lapacz.o obj/Galleries.o \
	obj/PCARec.o obj/SVMRec.o obj/ocv2pit.o obj/PPRec.o
else
POR_OBJ=obj/main.o obj/Lapacz.o obj/Galleries.o \
	obj/PCARec.o obj/SVMRec.o 
endif
VID_OBJ=obj/VideoCap.o obj/Lapacz.o
#run: detektor
#	./detektor kaskady/haarcascade_frontalface_default.xml \
#	galerie

all:  ${S_GAL} ${POR} ${M_GAL} ${VID}

${S_GAL}: ${S_GAL_OBJ}
	$(CC) -o ${S_GAL}  ${LIBS} ${S_GAL_OBJ} 

${POR}: ${POR_OBJ}
	$(CC) -o ${POR}  ${LIBS} ${POR_OBJ} ${PP_LIBS}

${M_GAL}: ${M_GAL_OBJ}
	$(CC) -o ${M_GAL}  ${LIBS} ${M_GAL_OBJ}

${VID}: ${VID_OBJ}
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

obj/PPRec.o: inc/Rec.hpp inc/PPRec.hpp inc/ocv2pit.hpp src/PPRec.cpp
	$(CC) -o obj/PPRec.o src/PPRec.cpp -c ${CFLAGS}

obj/VideoCap.o: inc/Lapacz.hpp  src/VideoCap.cpp
	$(CC) -o obj/VideoCap.o src/VideoCap.cpp -c ${CFLAGS}

clean:
	rm obj/* ${VID} ${M_GAL} ${S_GAL} ${POR}

