# Face recognition library
# Author: Michał Orynicz

SHELL=/bin/bash

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

# The file contains information if PittPatt library is present 
-include makefile.local

CFLAGS=${INCLUDES} -g -O0 -Wall -pedantic -Wno-sign-compare

CC=g++

# Names of programs. Can be freely changed
S_GAL=singleGalCreator
M_GAL=serialGalCreator
POR=comparator
VID=videoCreator
DET=detector
COMP=computer
TEST=tester

# objects needed for programs to compile
S_GAL_OBJ=obj/CapToGal.o obj/Catcher.o obj/Galleries.o
M_GAL_OBJ= obj/VideoToGal.o obj/Catcher.o obj/Galleries.o
VID_OBJ=obj/VideoCap.o obj/Catcher.o
POR_OBJ=obj/Comparator.o obj/Catcher.o obj/Galleries.o \
	obj/PCARec.o obj/SVMRec.o obj/ocv2pit.o obj/PPRec.o obj/Rec.o
DET_OBJ=obj/Detector.o obj/Catcher.o obj/Galleries.o \
	obj/PCARec.o obj/SVMRec.o obj/ocv2pit.o obj/PPRec.o obj/Rec.o
COMP_OBJ=obj/PCARec.o obj/SVMRec.o obj/PPRec.o obj/Computer.o \
	obj/Galleries.o obj/ocv2pit.o obj/Rec.o
TEST_OBJ=obj/PCARec.o obj/SVMRec.o obj/PPRec.o obj/Tester.o \
	obj/Galleries.o obj/ocv2pit.o obj/Rec.o

# All objects
OBJ= obj/CapToGal.o obj/Catcher.o obj/Galleries.o obj/Comparator.o  \
	obj/PCARec.o obj/SVMRec.o obj/VideoCap.o obj/VideoToGal.o \
	obj/ocv2pit.o obj/PPRec.o obj/Detector.o obj/Computer.o obj/Rec.o \
	obj/Tester.o

# folder containing galleries for buildGalleries target
GAL_FOLD=galleries

#folder containing videos for buildGalleries target
VID_FOLD=video

#number of photos to extract from videoas for each person
GAL_LIMIT=50

# macro creating base for list of labels for bulidGalleries
VIDS:=$(wildcard $(VID_FOLD)/*.avi)
# macro building list of labels for bulidGalleries
GALLERIES:=$(addprefix $(GAL_FOLD)/, $(notdir $(basename $(VIDS))))


#targets
all:  ${S_GAL} ${POR} ${M_GAL} ${VID} ${DET} ${COMP} ${TEST}

${S_GAL}: ${S_GAL_OBJ}
	$(CC) -o ${S_GAL}  ${LIBS} ${S_GAL_OBJ} 

${POR}: ${POR_OBJ}
	$(CC) -o ${POR}  ${LIBS} ${POR_OBJ} ${PP_LIBS}

${M_GAL}: ${M_GAL_OBJ}
	$(CC) -o ${M_GAL}  ${LIBS} ${M_GAL_OBJ}

${VID}: ${VID_OBJ}
	$(CC) -o ${VID} ${LIBS} ${VID_OBJ}

${DET}: ${DET_OBJ}
	$(CC) -o ${DET} ${LIBS} ${DET_OBJ} ${PP_LIBS}

${COMP}: ${COMP_OBJ}
	$(CC) -o ${COMP} ${LIBS} ${COMP_OBJ} ${PP_LIBS}

${TEST}: ${TEST_OBJ}
	$(CC) -o ${TEST} ${LIBS} ${TEST_OBJ} ${PP_LIBS}

${OBJ}: obj/%.o: src/%.cpp inc/*.hpp
	$(CC) -o $@ $< -c ${CFLAGS} 

clean:
	rm -f obj/* ${VID} ${M_GAL} ${S_GAL} ${POR} ${COMP} ${TEST} ${DET}
	make -C doc clean


buildPrecomputedGalleries: buildGalleries precompute

buildGalleries: folderPresence $(GALLERIES)

folderPresence:
	mkdir -p $(GAL_FOLD)
	mkdir -p $(VID_FOLD)


$(GALLERIES): $(GAL_FOLD)/%: $(VID_FOLD)/%.avi
	./${M_GAL} $(GAL_FOLD) $(notdir $@) $< $(GAL_LIMIT)

precompute: $(GALERIE)
	mkdir -p PCAData
	mkdir -p SVMData	
	mkdir -p PPRData

	./${COMP} $(GAL_FOLD) 

documentation:
	make -C doc

help:
	@echo "all - build all programs and objects"
	@echo "clean - clean all programs and objects and documentation"
	@echo "buildPrecomputedGalleries - build galleries and precompute"
	@echo "buildGalleries - build galleries based on videos in folder video"
	@echo "documentation - build documentation"

