CC=g++
CFLAGS="-Wno-cpp -O0"
CFLAGS-debug="-Wno-cpp -ggdb -O0 -p -D DEBUG "
TARGET=knl

DEPS=src/main.cpp

all:
	$(CC) $(CFLAGS) $(DEPS) -o $(TARGET)

debug:
	$(CC) $(CFLAGS-debug) $(DEPS) -o $(TARGET)-debug

clean:
	$(RM) knl