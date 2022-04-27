CC = g++
CXXFLAGS = -std=c++20 -O2
LDFLAGS = -lGL -lGLEW -lglfw -L/usr/local/lib

default: bouncing_ball simple_move follow_cursor fluids

all: bouncing_ball simple_move follow_cursor fluids game_of_life \
render_to_texture texture_demo standard_cube minigl_cube

fluids: fluids.o minigl.o
	$(CC) -o fluids.exe fluids.o minigl.o $(LDFLAGS)

game_of_life: game_of_life.o minigl.o
	$(CC) -o game_of_life.exe game_of_life.o minigl.o $(LDFLAGS)

render_to_texture: render_to_texture.o minigl.o 
	$(CC) -o render_to_texture.exe render_to_texture.o minigl.o $(LDFLAGS)

texture_demo: texture_demo.o minigl.o 
	$(CC) -o texture_demo.exe texture_demo.o minigl.o $(LDFLAGS)

standard_cube: standard_cube.o shader_loader.o 
	$(CC) -o standard_cube.exe standard_cube.o shader_loader.o $(LDFLAGS)

minigl_cube: minigl_cube.o minigl.o
	$(CC) -o minigl_cube.exe minigl_cube.o minigl.o $(LDFLAGS)

sandbox: sandbox.o minigl.o 
	$(CC) -o sandbox.exe sandbox.o minigl.o $(LDFLAGS)

bouncing_ball: bouncing_ball.o minigl2d.o minigl.o 
	$(CC) -o bouncing_ball.exe bouncing_ball.o minigl2d.o minigl.o $(LDFLAGS)

simple_move: simple_move.o minigl2d.o minigl.o
	$(CC) -o simple_move.exe simple_move.o minigl2d.o minigl.o $(LDFLAGS)

follow_cursor: follow_cursor.o minigl2d.o minigl.o
	$(CC) -o follow_cursor.exe follow_cursor.o minigl2d.o minigl.o $(LDFLAGS)


follow_cursor: minigl2d.hpp minigl.hpp
simple_move: minigl2d.hpp minigl.hpp
bouncing_ball.o: minigl2d.hpp minigl.hpp
minigl2d.o: minigl2d.hpp minigl.hpp
texture_demo.o: minigl.hpp
sandbox.o: minigl.hpp
standard_cube.o: shader_loader.hpp
shader_loader.o: shader_loader.hpp
minigl.o: minigl.hpp
render_to_texture.o: minigl.hpp
game_of_life.o: minigl.hpp
fluids.o: minigl.hpp

clean:
	rm -f *.exe *.exe.* *.o

.PHONY: cleanCC = g++
