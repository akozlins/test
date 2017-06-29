all:
	g++ -Wall -Wextra \
	-mfpmath=sse -msse2 \
	-O2 \
	-lmpfr \
	-o main main.cpp
	/usr/bin/time -v ./main
