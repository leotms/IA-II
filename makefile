main:		main.cc othello_cut.h
		g++ -O3 -Wall -std=c++11 -o main main.cc
		g++ -O3 -Wall -std=c++11 -o test test.cc

clean:
		rm -f main core *~

