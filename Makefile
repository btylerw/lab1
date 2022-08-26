
all: lab1 lab1v

lab1: lab1.cpp
	g++ lab1.cpp -Wall -olab1 -lX11 -lGL -lGLU -lm

lab1v:	lab1_vertical.cpp
	g++ lab1_vertical.cpp -Wall -olab1v -lX11 -lGL -lGLU -lm

clean:
	rm -f lab1
	rm -f lab1v

