CC=g++
CFLAGS="-Wno-cpp -ggdb -O0 -p"

TARGET=knl

DEPS=src/main.cpp

all:
	$(CC) $(CFLAGS) $(DEPS) -o $(TARGET)

clean:
	$(RM) knl