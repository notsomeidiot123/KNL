CC=g++
CFLAGS=-Wno-cpp -Ofast
CFLAGS-debug=-Wno-cpp -g -O0 -D DEBUG
TARGET=knl

DEPS=src/main.cpp

all:
	$(CC) $(CFLAGS) $(DEPS) -o $(TARGET)

debug:
	$(RM) core*.out
	$(CC) $(CFLAGS-debug) $(DEPS) -o $(TARGET)-debug

clean:
	$(RM) knl*
	$(RM) gmon.out
	$(RM) core*
	$(RM) debug.hd