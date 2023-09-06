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

void mem_handler(uint8_t* addr)
{
}
