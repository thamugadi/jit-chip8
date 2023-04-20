jit-chip8 : src/main.c src/emulator.c src/jit.c src/interpreter.c src/include/chip8.h
	mkdir -p bin
	gcc -I src/include -masm=intel src/*.c -o bin/jit-chip8
clean:
	rm -rf bin

run:
	./bin/jit-chip8
