jit-chip8 : src/main.c src/emulator.c src/jit.c src/interpreter.c src/include/chip8.h
	mkdir -p bin
	gcc -Ofast -funsafe-math-optimizations -free -fno-stack-protector -I src/include -lglut -lGLU -lGL -no-pie -masm=intel -Wno-incompatible-pointer-types -Wno-int-conversion src/*.c -o bin/jit-chip8
clean:
	rm -rf bin

run:
	./bin/jit-chip8 test

gdb:
	gdb --args ./bin/jit-chip8 test
