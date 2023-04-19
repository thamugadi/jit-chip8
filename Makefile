jit : src/main.c src/include/chip8.h
	mkdir bin
	gcc -I src/include -masm=intel src/main.c -o bin/jit
