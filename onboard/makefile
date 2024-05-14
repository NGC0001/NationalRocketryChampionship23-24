CC = gcc
CXX = g++

CFLAG = -fsanitize=address -g
C_CFLAG = $(CFLAG)
CXX_CFLAG = $(CFLAG) -std=c++17 -Wall -Wextra -Wfloat-equal -Wpedantic -Wvla -Werror

LFLAG = -lm -li2c -static-libasan

all: geiger altimeter

geiger: geiger.cpp
	$(CXX) $< $(CXX_CFLAG) $(LFLAG) -o $@

bmp.o: bmp3.c bmp3.h bmp3_defs.h
	$(CC) -c $< $(C_CFLAG) -o $@

ada.o: adafruit_bmp3xx.cpp adafruit_bmp3xx.h bmp.o
	$(CXX) -c $< $(CXX_CFLAG) -o $@

altimeter: altimeter.cpp ada.o bmp.o
	$(CXX) $^ $(CXX_CFLAG) $(LFLAG) -o $@

clean:
	rm -f *.o geiger altimeter
