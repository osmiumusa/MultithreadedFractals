all: julia

julia: main.c bitmap.h complex.h
	gcc main.c -o julia -lm -pthread -g

test: julia
	./julia 1024 -0.4 0.6 -2 2 -2 2 output.bmp 2 500 2

clean:
	rm -rf julia output.bmp