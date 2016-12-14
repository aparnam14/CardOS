
#include <stdint.h>
#include "string.h"
#include "memory.h"
#include "log.h"

extern uint32_t kernel_end; 
uint32_t next_free = (uint32_t)&kernel_end;

void *kmalloc(size_t size) {
  void *mem =  (void*)next_free;
  next_free += size;
  return mem;
}

void *kmalloc_page() {
  if (next_free & 0xfffff000) {
    next_free &= 0xfffff000;
    next_free += 0x1000; 
  }
  debug("allocated page at %x", next_free);
  return kmalloc(0x1000);
}
