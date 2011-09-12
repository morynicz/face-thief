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
	-g

run: detektor
	./detektor kaskady/haarcascade_frontalface_default.xml \
	galerie

detektor: src/*.cpp inc/*.hpp obj/main.o obj/Lapacz.o obj/Galleries.o
	gcc -o detektor  obj/*.o ${CFLAGS}

obj/main.o: src/main.cpp inc/*.hpp
	gcc -o obj/main.o src/main.cpp -c ${CFLAGS}

obj/Lapacz.o: inc/Lapacz.hpp src/Lapacz.cpp
	gcc -o obj/Lapacz.o src/Lapacz.cpp -c ${CFLAGS}

obj/Galleries.o: inc/Galleries.hpp
	gcc -o obj/Galleries.o src/Galleries.cpp -c ${CFLAGS}

clean:
	rm obj/* detektor