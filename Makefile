jit : src/main.c src/include/chip8.h
	mkdir -p bin
	gcc -I src/include -masm=intel src/main.c -o bin/jit
clean:
	rm -rf bin
