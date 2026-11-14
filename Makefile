CXX = clang++
CC = clang

INCLUDE = -Iinclude
LIBS = -lglfw -lGL

CXXFLAGS = -std=c++17 $(INCLUDE) -Wall -Wextra -MMD -MP
CFLAGS = $(INCLUDE) -MMD -MP

TARGET = bin/main

SRCS_CPP = src/main.cpp
SRCS_C = src/glad.c
OBJS = obj/main.o obj/glad.o
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p bin
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

obj/main.o: src/main.cpp
	mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/glad.o: src/glad.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

run: all
	./$(TARGET)

clean:
	rm -rf obj/* bin/*

.PHONY: all run clean