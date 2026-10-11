all:
	g++ -I src/include -L src/lib -o main main.cpp chip8.cpp -lmingw32 -lSDL3 -mconsole