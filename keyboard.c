#include <stdbool.h>

#include "keyboard.h"
#include "isr.h"
#include "io.h"
#include "string.h"
#include "log.h"

#define KBD_DATA_PORT 0x60

static int scan_map_no_shift[] = { -1 /* keyboard error */,
  1 /* esc */, '1', '2', '3', '4',
  '5', '6', '7', '8', '9', '0', '-', '=', 127 /* delete */,
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
  '[', ']', '\n', -1, /* ctrl */ 'a', 's', 'd', 'f', 'g',
  'h', 'j', 'k', 'l', ';', '\'', '`', -1 /* lshift */,
  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
  -1 /* right shift */, ' ', -1 /* caps lock */ };

static int scan_map_shift[] = { -1 /* keyboard error */,
  1, /* esc */ '!', '@', '#', '$', '%', '^', '&', '*',
  '(', ')', '_', '+', 127 /* delete */, '\t', 'Q', 'W',
  'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
  '\n', -1, /* ctrl */ 'A', 'S', 'D', 'F', 'G', 'H',
  'J', 'K', 'L', ':', '"', '~', -1 /* left shift */,
  '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
  -1 /* right shift */, ' ', -1 /* caps lock */ };

static bool shift = false;

#define KBD_SCAN_LSHIFT_KEY_DOWN 42
#define KBD_SCAN_LSHIFT_KEY_UP 170
#define KBD_SCAN_RSHIFT_KEY_DOWN 54
#define KBD_SCAN_RSHIFT_KEY_UP 182


static void keyboard_cb(registers_t regs) {
  unsigned char scan_code = inb(KBD_DATA_PORT);
  char c = '0';

  if (scan_code == KBD_SCAN_LSHIFT_KEY_DOWN ||
      scan_code == KBD_SCAN_RSHIFT_KEY_DOWN) {
    shift = true;
  } else if (scan_code == KBD_SCAN_LSHIFT_KEY_UP ||
    scan_code == KBD_SCAN_RSHIFT_KEY_UP) {
    shift = false;
  } else {
    if (scan_code < 128) {
      if (!shift)
        c = scan_map_no_shift[scan_code];
      else
        c = scan_map_shift[scan_code];

      if (c != -1) {
        printf("%c", c);
      }
    }
    else {
      debug("scan_code=%x for the pressed key", scan_code);
    }
  }
}

void init_keyboard() {
  debug("initializing keyboard.");
  register_interrupt_handler(IRQ1, &keyboard_cb);
}
