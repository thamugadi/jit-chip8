#include <chip8.h> 

void keyboardUp(SDL_Keycode key)
{
        if (key == SDLK_3) context.keys[0] = 0;
	else if (key == SDLK_4) context.keys[1] = 0;
	else if (key == SDLK_5) context.keys[2] = 0;
	else if (key == SDLK_6) context.keys[3] = 0;
	else if (key == SDLK_e) context.keys[4] = 0;
	else if (key == SDLK_r) context.keys[5] = 0;
	else if (key == SDLK_t) context.keys[6] = 0;
	else if (key == SDLK_y) context.keys[7] = 0;
	else if (key == SDLK_d) context.keys[8] = 0;
	else if (key == SDLK_f) context.keys[9] = 0;
	else if (key == SDLK_g) context.keys[10] = 0;
	else if (key == SDLK_h) context.keys[11] = 0;
	else if (key == SDLK_c) context.keys[12] = 0;
	else if (key == SDLK_v) context.keys[13] = 0;
	else if (key == SDLK_b) context.keys[14] = 0;
	else if (key == SDLK_n) context.keys[15] = 0;
}

void keyboardDown(SDL_Keycode key)
{
        if (key == SDLK_3) context.keys[0] = 1;
	else if (key == SDLK_4) context.keys[1] = 1;
	else if (key == SDLK_5) context.keys[2] = 1;
	else if (key == SDLK_6) context.keys[3] = 1;
	else if (key == SDLK_e) context.keys[4] = 1;
	else if (key == SDLK_r) context.keys[5] = 1;
	else if (key == SDLK_t) context.keys[6] = 1;
	else if (key == SDLK_y) context.keys[7] = 1;
	else if (key == SDLK_d) context.keys[8] = 1;
	else if (key == SDLK_f) context.keys[9] = 1;
	else if (key == SDLK_g) context.keys[10] = 1;
	else if (key == SDLK_h) context.keys[11] = 1;
	else if (key == SDLK_c) context.keys[12] = 1;
	else if (key == SDLK_v) context.keys[13] = 1;
	else if (key == SDLK_b) context.keys[14] = 1;
	else if (key == SDLK_n) context.keys[15] = 1;
}

