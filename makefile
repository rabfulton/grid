CFLAGS =-std=c99 -Wall -O3 -g `sdl2-config --cflags`
LFLAGS = `sdl2-config --libs` -L/usr/local/include/ -lGLEW -lSDL2_image -lm -lSDL2_mixer -lSDL2_ttf -lGL -lGLU
OBJS   = main.o draw.o vector.o fileops.o editor.o
PROG = a.out
CXX = clang

# top-level rule to create the program.
all: $(OBJS) $(PROG)

# compiling other source files.
%.o: %./c main.h
	$(CXX) $(CFLAGS) -c -s $<

# linking the program.
$(PROG): $(OBJS)
	$(CXX) $(OBJS) -o $(PROG) -g $(LFLAGS)

# cleaning everything that can be automatically recreated with "make".
clean:
	rm $(PROG) *.o
