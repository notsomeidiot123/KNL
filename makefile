CC=g++
CFLAGS="-Wno-cpp"

TARGET=knl

DEPS=src/main.cpp

all:
	$(CC) $(CFLAGS) $(DEPS) -o $(TARGET)

clean:
	$(RM) knl