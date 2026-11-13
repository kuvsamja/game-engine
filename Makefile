CXX = clang++
CC = clang

INCLUDE = -Iinclude
LIBS = -lglfw -lGL

CXXFLAGS = -std=c++17 $(INCLUDE) -Wall -Wextra
CFLAGS = $(INCLUDE)

TARGET = bin/main

SRCS_CPP = src/main.cpp
SRCS_C = src/glad.c
OBJS = obj/main.o obj/glad.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

obj/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/glad.o: src/glad.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f obj/*.o $(TARGET)

.PHONY: all run clean