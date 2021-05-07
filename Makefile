CC=g++
all:
	$(CC) -pthread -o server server.cpp -I.