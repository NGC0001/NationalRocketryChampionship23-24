CC = gcc

CXX = g++
CXX_CFLAG = -std=c++17 -Wall -Wextra -Wfloat-equal -Wpedantic -Wvla -Werror

geiger: geiger.cpp
	$(CXX) $(CXX_CFLAG) $< -o $@

bmp.o: bmp3.c bmp3.h bmp3_defs.h
	$(CC) -c $< -o $@

ada.o: Adafruit_BMP3XX.cpp Adafruit_BMP3XX.h common.h
	$(CXX) -c $< -o $@

altimeter: bmp3xx_simpletest.cpp ada.o bmp.o common.h
	$(CXX) bmp3xx_simpletest.cpp ada.o bmp.o -o $@

clean:
	rm -f *.o geiger altimeter
