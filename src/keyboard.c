#include <chip8.h> 

#define KEY(a) (key == a)

void keyboardUp(uint8_t key, int x, int y)
{
        if (key == '3') context.keys[0] = 0;
        if (key == '4') context.keys[1] = 0;
        if (key == '5') context.keys[2] = 0;
        if (key == '6') context.keys[3] = 0;
        if (KEY('E')) context.keys[4] = 0;
        if (KEY('R')) context.keys[5] = 0;
        if (KEY('T')) context.keys[6] = 0;
        if (KEY('Y')) context.keys[7] = 0;
        if (KEY('D')) context.keys[8] = 0;
        if (KEY('F')) context.keys[9] = 0;
        if (KEY('G')) context.keys[10] = 0;
        if (KEY('H')) context.keys[11] = 0;
        if (KEY('C')) context.keys[12] = 0;
        if (KEY('V')) context.keys[13] = 0;
        if (KEY('B')) context.keys[14] = 0;
        if (KEY('N')) context.keys[15] = 0;
}

void keyboardDown(uint8_t key, int x, int y)
{
        if (key == '3') context.keys[0] = 1;
        if (key == '4') context.keys[1] = 1;
        if (key == '5') context.keys[2] = 1;
        if (key == '6') context.keys[3] = 1;
        if (KEY('E')) context.keys[4] = 1;
        if (KEY('R')) context.keys[5] = 1;
        if (KEY('T')) context.keys[6] = 1;
        if (KEY('Y')) context.keys[7] = 1;
        if (KEY('D')) context.keys[8] = 1;      
        if (KEY('F')) context.keys[9] = 1;
        if (KEY('G')) context.keys[10] = 1;
        if (KEY('H')) context.keys[11] = 1;
        if (KEY('C')) context.keys[12] = 1;
        if (KEY('V')) context.keys[13] = 1;
        if (KEY('B')) context.keys[14] = 1;
        if (KEY('N')) context.keys[15] = 1;

}

