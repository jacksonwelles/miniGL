CC = g++
CXXFLAGS = -std=c++20 -O2
LDFLAGS = -lGL -lGLEW -lglfw3 -L/usr/local/lib

default: all

all: minigl_cube standard_cube

standard_cube: standard_cube.o shader_loader.o 
	$(CC) -o standard_cube.exe standard_cube.o shader_loader.o $(LDFLAGS)

minigl_cube: minigl_cube.o minigl.o
	$(CC) -o minigl_cube.exe minigl_cube.o minigl.o $(LDFLAGS)

sandbox: sandbox.o minigl.o 
	$(CC) -o sandbox.exe sandbox.o minigl.o $(LDFLAGS)

sandbox.o: minigl.hpp
standard_cube.o: shader_loader.hpp minigl.hpp
shader_loader.o: shader_loader.hpp
minigl.o: minigl.hpp

clean:
	rm -f *.exe *.exe.* *.o

.PHONY: clean