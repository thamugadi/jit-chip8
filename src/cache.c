#include <chip8.h>

struct cache_entry cache[CACHE_SIZE];

void update_cache(uint8_t* addr, int n, uint16_t pc, uint64_t bytes, uint8_t* mem_address)
{
        if (recompiled_block < CACHE_SIZE)
        {
                cache[recompiled_block] = (struct cache_entry){addr, pc, 0, n, bytes};
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
                cache[index] = (struct cache_entry){addr, pc, 0, n, bytes, mem_address};
        }
}

struct access_cache_s access_cache(uint16_t pc_addr, int update)
{
        for (int i = 0; i < CACHE_SIZE; i++)
        {
                if (cache[i].pc == pc_addr) // hit
                {
			if (update)
			{
				cache[i].freq++;
			}
                        return (struct access_cache_s)
			  {1, cache[i].n, cache[i].addr, cache[i].emitted_bytes, cache[i].pc, cache[i].mem_address}; 
                }
        }
	return (struct access_cache_s){0,0,0,0};
}
