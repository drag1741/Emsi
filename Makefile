IDIR = ./include
SOURCEDIR = ./src

CC = gcc
CFLAGS = -g -I $(IDIR) $(SRC) -lz -lpthread -ldl

SOURCES = $(wildcard $(SOURCEDIR)/*.c)

make: $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) ./src/sqlite3.o
