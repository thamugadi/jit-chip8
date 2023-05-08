#include <chip8.h>

struct cache_entry cache[CACHE_SIZE];

void update_cache(uint8_t* addr, int n, uint16_t pc)
{
        if (recompiled_instr < CACHE_SIZE)
        {
                cache[recompiled_instr] = (struct cache_entry){addr, pc, 0, n};
        }
        else
        {
                // LFU algorithm
                int index = 0;
                int min = cache[index].freq;
                for (int i = 1; i < CACHE_SIZE; i++)
                {
                        if (cache[i].freq < min)
                        {
                                min = cache[i].freq;
                                index = i;
                        }
                }
		munmap(cache[index].addr, (cache[index].n) * MAX_EMITTED);
                cache[index] = (struct cache_entry){addr, pc, 0, n};
        }
}

struct access_cache_s access_cache(uint16_t pc_addr)
{
        for (int i = 0; i < CACHE_SIZE; i++)
        {
                if (cache[i].pc == pc_addr) // hit
                {
                        cache[i].freq++;
                        return (struct access_cache_s){1, cache[i].n, cache[i].addr}; 
                }
        }
	return (struct access_cache_s){0,0,0};
}

