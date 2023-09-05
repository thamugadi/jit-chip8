#include <chip8.h>

/* instructions involving reading chip8 memory and not 7 bytes long:
 * 88 21 
 * 8a 98 (32LE)
 * 88 1c 01
 * 8a 1c 08
 * 88 98 (32LE)
 * 
*/

void segfault_handler(int sig_n, siginfo_t *info, void *ctx)
{
        printf("Segfault handler\n");
        if (!exec_jit)
        {
                printf("Segmentation fault\n");
                exit(-1);
        }
        mprotect(&context.memory, MEMSIZE, PROT_READ | PROT_WRITE);
        exit(0);
}

