CC = g++
CFLAGS = -std=c++11 -O0 -g -Wall -Wextra -pedantic
LIBS = -ldnest4 -lpthread

default:
	$(CC) $(CFLAGS) -I$(DNEST4_PATH) -c *.cpp Models/*.cpp
	$(CC) -L$(DNEST4_PATH)/DNest4/code -o main *.o $(LIBS)
	ar rcs liblines.a *.o
	rm -rf *.o *.gch


