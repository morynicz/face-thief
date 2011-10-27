SHELL=/bin/bash
PITPATT_HOST="rab"
HOST=$${HOSTNAME}

INCLUDES= -Iinc \
	-I /usr/local/include/opencv2 \
	-I /usr/include/boost 

LIBS=-lopencv_core \
	-lopencv_highgui \
	-lopencv_ml \
	-lopencv_objdetect \
	-lopencv_gpu  \
	-lboost_thread \
	-lboost_filesystem 

ifeq (${HOST}, ${PITTPATT_HOST})
PP_LIBS= -lpittpatt_ftr_sdk \
	-lpittpatt_raw_image \
	-lpittpatt_recognition_core  
endif

CFLAGS=${INCLUDES} -g -O0 -Wall -pedantic -Wno-sign-compare

CC=g++

S_GAL=kreatorGalerii
M_GAL=masGalKreator
POR=porownywacz
VID=videoKreator
DET=detektor

S_GAL_OBJ=obj/CapToGal.o obj/Lapacz.o obj/Galleries.o
M_GAL_OBJ= obj/VideoToGal.o obj/Lapacz.o obj/Galleries.o
VID_OBJ=obj/VideoCap.o obj/Lapacz.o
POR_OBJ=obj/Comparator.o obj/Lapacz.o obj/Galleries.o \
	obj/PCARec.o obj/SVMRec.o obj/ocv2pit.o obj/PPRec.o
DET_OBJ=obj/Detector.o obj/Lapacz.o obj/Galleries.o \
	obj/PCARec.o obj/SVMRec.o obj/ocv2pit.o obj/PPRec.o

OBJ= obj/CapToGal.o obj/Lapacz.o obj/Galleries.o obj/Comparator.o  \
	obj/PCARec.o obj/SVMRec.o obj/VideoCap.o obj/VideoToGal.o \
	obj/ocv2pit.o obj/PPRec.o obj/Detector.o

all:  ${S_GAL} ${POR} ${M_GAL} ${VID} ${DET}

${S_GAL}: ${S_GAL_OBJ}
	$(CC) -o ${S_GAL}  ${LIBS} ${S_GAL_OBJ} 

${POR}: ${POR_OBJ}
	$(CC) -o ${POR}  ${LIBS} ${POR_OBJ} ${PP_LIBS}

${M_GAL}: ${M_GAL_OBJ}
	$(CC) -o ${M_GAL}  ${LIBS} ${M_GAL_OBJ}

${VID}: ${VID_OBJ}
	$(CC) -o ${VID} ${LIBS} ${VID_OBJ}

${DET}: ${DET_OBJ}
	$(CC) -o ${DET} ${LIBS} ${DET_OBJ}

${OBJ}: obj/%.o: src/%.cpp inc/*.hpp
	$(CC) -o $@ $< -c ${CFLAGS} 



clean:
	rm obj/* ${VID} ${M_GAL} ${S_GAL} ${POR}

