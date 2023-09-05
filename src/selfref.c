#include <chip8.h>

/* instructions involving reading chip8 memory and not 7 bytes long:
 * 88 21 
 * 8a 98 (32LE)
 * 88 1c 01
 * 8a 1c 08
 * 88 98 (32LE)
 * 
*/
/* other instructions involving reading chip8 memory are all 7 bytes long.*/
/* * used registers: rax, rbx, rcx, rdx, r10 */

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

