CXX = clang++
CC  = clang

INCLUDE = -Iinclude
LIBS    = -lglfw -lGL

TARGET = bin/main
OBJS   = obj/main.o obj/glad.o
DEPS   = $(OBJS:.o=.d)

.DEFAULT_GOAL := all

CXXFLAGS = -std=c++17 $(INCLUDE) -Wall -Wextra -MMD -MP -O3
CFLAGS   = $(INCLUDE) -MMD -MP -O3

all: $(TARGET)

debug: CXXFLAGS = -std=c++17 $(INCLUDE) -Wall -Wextra -MMD -MP -g -O0
debug: CFLAGS   = $(INCLUDE) -MMD -MP -g -O0
debug: clean $(TARGET)

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

rundebug: debug
	./$(TARGET)

clean:
	rm -rf obj/* bin/*

.PHONY: all debug run rundebug clean