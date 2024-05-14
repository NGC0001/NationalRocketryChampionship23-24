CC = gcc

CXX = g++
CXX_CFLAG = -std=c++17 -Wall -Wextra -Wfloat-equal -Wpedantic -Wvla -Werror

LFLAG = -lm

all: geiger altimeter

geiger: geiger.cpp
	$(CXX) $(CXX_CFLAG) $< -o $@

bmp.o: bmp3.c bmp3.h bmp3_defs.h
	$(CC) -c $< -o $@

ada.o: adafruit_bmp3xx.cpp adafruit_bmp3xx.h bmp.o
	$(CXX) -c $< -o $@

altimeter: altimeter.cpp ada.o bmp.o
	$(CXX) $^ $(LFLAG) -o $@

clean:
	rm -f *.o geiger altimeter
