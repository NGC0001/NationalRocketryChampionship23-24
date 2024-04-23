CC = gcc

CXX = g++
CXX_CFLAG = -std=c++17 -Wall -Wextra -Wfloat-equal -Wpedantic -Wvla -Werror

LFLAG = -lm

all: geiger altimeter

geiger: geiger.cpp
	$(CXX) $(CXX_CFLAG) $< -o $@

bmp.o: bmp3.c bmp3.h bmp3_defs.h
	$(CC) -c $< -o $@

ada.o: Adafruit_BMP3XX.cpp Adafruit_BMP3XX.h
	$(CXX) -c $< -o $@

altimeter: altimeter.cpp ada.o bmp.o
	$(CXX) $^ $(LFLAG) -o $@

clean:
	rm -f *.o geiger altimeter
