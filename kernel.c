#include <stdbool.h>

#include "log.h"
#include "descriptor_tables.h"
#include "fb.h"
#include "keyboard.h"
#include "multiboot.h"
#include "serial.h"
#include "string.h"
#include "paging.h"
#include "isr.h"


bool irq3_detect = false;
bool irq4_detect = false;

void irq3_handler(registers_t regs) {
  debug("IRQ triggered 3, handled.");
  irq3_detect = true;
}

void irq4_handler(registers_t regs) {
  debug("IRQ triggered 4, handled.");
  irq4_detect = true;
}

void kernel(multiboot_info_t *info) {
  fb_clear();
  debug("Welcome to CardOS!");
  debug("Little OS Kernel Booting!");

  char greeting[] = "Welcome to CardOS.. kernel Booting!";
  debug("Initializing the serial driver");
  serial_write(greeting, strlen(greeting));

  debug("multiboot header flags: %x", info->flags);

  init_descriptor_tables();

  debug("Triggering IRQ 3 and 4...");
  register_interrupt_handler(3, irq3_handler);
  register_interrupt_handler(4, irq4_handler);
  asm volatile ("int $0x3");
  asm volatile ("int $0x4");
  if (!irq3_detect) debug("irqs not configured, system crashig!!");
  if (!irq4_detect) debug("irqs not configured, system crashing!!");

  init_keyboard();
  init_paging();
  return;
}
