#include <chip8.h>

#define PERIOD 0
#define DRAW_POS(px, py) \
        glVertex2i(px, py); \
        glVertex2i(px+PIXELSIZE, py); \
        glVertex2i(px+PIXELSIZE, py+PIXELSIZE); \
        glVertex2i(px, py+PIXELSIZE);

SDL_Window* window;
SDL_GLContext gl_context;

int cycle = 0;
int exec_jit = 0;
void* ptrMainLoopEvent;
uint8_t font[0x50] =
{ 
        0xF0, 0x90, 0x90, 0x90, 0xF0,
        0x20, 0x60, 0x20, 0x20, 0x70,
        0xF0, 0x10, 0xF0, 0x80, 0xF0,
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
        0x90, 0x90, 0xF0, 0x10, 0x10,
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
        0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x80, 0xF0,
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        0xF0, 0x80, 0xF0, 0x80, 0x80
};

void display()
{
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_QUADS);

        for (int i = 0; i < WIDTH; i++)
        {
                for (int j = 0; j < HEIGHT; j++)
                {
                        if (context.gfx[i][j])
                        {
                                DRAW_POS(i, j);
                        }

                }
        }
        glEnd();
        SDL_GL_SwapWindow(window);
}

void initGL()
{
        gluOrtho2D(0, WIDTH * PIXELSIZE, 0, HEIGHT * PIXELSIZE);
        glMatrixMode(GL_MODELVIEW);
}

void cpu(int frequency)
{
	int i = frequency;
	while (i--)
	{
        	emulate_basic_block();
	}
        if (context.dt)
        {
                context.dt--;
        }

}

int main(int argc, char** argv)
{
        context.memory = mmap(0, 0x1000, 7, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        memset(context.memory, 0, 2 * 0x500);
        memset(cache, 0xff, sizeof(struct cache_entry)*CACHE_SIZE);

        if (argc < 2)
        {
                puts("format : jit-chip8 <chip-8 rom>");
                return 1;
        }

        FILE *file = fopen(argv[1], "rb");

        if (!file)
        {
                fprintf(stderr, "Error: Failed to open file '%s'.\n", argv[1]);
                return 2;
        }
        uint8_t buffer[0x800];
        size_t bytes_read = fread((uint8_t*)(context.memory+0x200), 1, MEMSIZE*2 - 0x200, file);

        if (bytes_read == 0x800 && !feof(file)) 
        {
                fprintf(stderr, "Error: File size is larger than %x. \n", MEMSIZE*2 - 0x200);
                fclose(file);
                return 3;
        }

        fclose(file);

        context.sp = 0;
        context.pc = 0x200;
        context.I = 0;
        context.dt = 0;
        context.st = 0;

        memset(context.V, 0, 16);
        memset(context.gfx, 0, WIDTH * HEIGHT);
        memset(context.stack, 0, 2 * 16);
        memset(context.keys, 0, 16);

        uint8_t* memptr = context.memory;
        for (int i = 0; i < 0x50; i++)
        {
                *memptr++ = font[i];
        }

        uint8_t* ptr = context.memory;

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
                fprintf(stderr, "SDL_Error: %s\n", SDL_GetError());
                return 1;
        }

        window = SDL_CreateWindow("CHIP-8 JIT compiler", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 16*WIDTH * PIXELSIZE, 16*HEIGHT * PIXELSIZE, SDL_WINDOW_OPENGL);

        if (!window)
        {
                fprintf(stderr, "SDL_Error: %s\n", SDL_GetError());
                return 2;
        }

        gl_context = SDL_GL_CreateContext(window);

        initGL();

        SDL_Event e;
	int quit = 0;
	while(!quit)
	{
	
		while (SDL_PollEvent(&e))
		{
    			if (e.type == SDL_QUIT) 
			{
				quit = 1;
			}
    			else if (e.type == SDL_KEYDOWN) keyboardDown(e.key.keysym.sym);
    			else if (e.type == SDL_KEYUP) keyboardUp(e.key.keysym.sym);
		}
		cpu(100);
		display();
	}

        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
}
