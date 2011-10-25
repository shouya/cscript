all: test


lib: ../mapmodel.cpp ../export.h
	gcc -Wall -shared -o librtc.so ../mapmodel.cpp -lstdc++

test: unittest.c lib
	gcc -Wall -o unittest unittest.c -L. -lrtc