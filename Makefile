IDIR = ./include
SOURCEDIR = ./src

CC = gcc
CFLAGS = -I $(IDIR) $(SRC) -lz -lpthread -ldl

SOURCES = $(wildcard $(SOURCEDIR)/*.c)

make: $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) ./src/sqlite3.o
