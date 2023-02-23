CC=g++
CFLAGS="-Wno-cpp -ggdb -O0 -p"
CFLAGS-debug="-Wno-cpp -ggdb -O0 -p"
TARGET=knl

DEPS=src/main.cpp

all:
	$(CC) $(CFLAGS) $(DEPS) -o $(TARGET)

debug:
	$(CC) $(CFLAGS) $(DEPS) -o $(TARGET)-debug

clean:
	$(RM) knl