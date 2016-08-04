CC = g++
CFLAGS = -std=c++11 -O3 -Wall -Wextra -pedantic
LIBS = -ldnest4 -lpthread

default:
	$(CC) $(CFLAGS) -I$(DNEST4_PATH) -c main.cpp LiNeS.cpp ClassicLogger.cpp Models/SimpleExample.cpp
	$(CC) -L$(DNEST4_PATH)/DNest4/code -o main *.o $(LIBS)
	ar rcs liblines.a *.o
	rm -rf *.o

