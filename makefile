CC = g++
CXXFLAGS = -std=c++20 -O2 -Iinc 
LDFLAGS = -lGL -lGLEW -lglfw -L/usr/local/lib
VPATH = src:inc:demo:demo/classic
TARGETS = fluids game_of_life render_to_texture texture_demo standard_cube \
minigl_cube sandbox bouncing_ball simple_move follow_cursor pong paint

default: bouncing_ball simple_move follow_cursor fluids pong paint

all: bouncing_ball simple_move follow_cursor fluids game_of_life \
render_to_texture texture_demo standard_cube minigl_cube sandbox pong paint

fluids: fluids.o minigl.o
	$(CC) -o fluids fluids.o minigl.o $(LDFLAGS)

game_of_life: game_of_life.o minigl.o
	$(CC) -o game_of_life game_of_life.o minigl.o $(LDFLAGS)

render_to_texture: render_to_texture.o minigl.o 
	$(CC) -o render_to_texture render_to_texture.o minigl.o $(LDFLAGS)

texture_demo: texture_demo.o minigl.o 
	$(CC) -o texture_demo texture_demo.o minigl.o $(LDFLAGS)

standard_cube: standard_cube.o shader_loader.o 
	$(CC) -o standard_cube standard_cube.o shader_loader.o $(LDFLAGS)

minigl_cube: minigl_cube.o minigl.o
	$(CC) -o minigl_cube minigl_cube.o minigl.o $(LDFLAGS)

sandbox: sandbox.o minigl.o 
	$(CC) -o sandbox sandbox.o minigl.o $(LDFLAGS)

bouncing_ball: bouncing_ball.o minigl2d.o minigl.o 
	$(CC) -o bouncing_ball bouncing_ball.o minigl2d.o minigl.o $(LDFLAGS)

simple_move: simple_move.o minigl2d.o minigl.o
	$(CC) -o simple_move simple_move.o minigl2d.o minigl.o $(LDFLAGS)

follow_cursor: follow_cursor.o minigl2d.o minigl.o
	$(CC) -o follow_cursor follow_cursor.o minigl2d.o minigl.o $(LDFLAGS)

pong: pong.o minigl2d.o minigl.o
	$(CC) -o pong pong.o minigl2d.o minigl.o $(LDFLAGS)

paint: paint.o minigl2d.o minigl.o
	$(CC) -o paint paint.o minigl2d.o minigl.o $(LDFLAGS)

paint: minigl2d.hpp minigl.hpp
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
	rm -f *.exe *.exe.* *.o $(TARGETS)

.PHONY: cleanCC = g++
