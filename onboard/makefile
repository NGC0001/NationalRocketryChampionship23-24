CXX = g++
CXX_CFLAG = -std=c++17 -Wall -Wextra -Wfloat-equal -Wpedantic -Wvla -Werror

geiger: geiger.cpp
	$(CXX) $(CXX_CFLAG) $^ -o $@

clean:
	rm -f geiger
