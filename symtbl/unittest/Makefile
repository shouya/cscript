test: unittest.c ../export.h ../mapmodel.cpp
	gcc -c unittest.c -o unittest.o -Wall
	gcc -c ../mapmodel.cpp -o mapmodel.o -Wall -lstdc++
	gcc -o test unittest.o mapmodel.o -Wall
