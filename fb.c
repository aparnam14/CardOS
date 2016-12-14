#include <stddef.h>
#include <stdint.h>

#include "io.h"
#include "fb.h"
#include "string.h"
#include "serial.h"

#define FRAMEBUFFER_ADDR 0x000B8000;
#define FB_WIDTH 80
#define FB_HEIGHT 25
#define TAB "  "

static unsigned int fb_x = 0;
static unsigned int fb_y = 0;

/**
 * fb_write_cell:
 * Writes a character with the specified fg and bg
 */
void fb_write_cell(short i, char c, unsigned char fg, unsigned char bg) {
  unsigned char *fb = (unsigned char*)FRAMEBUFFER_ADDR;
  fb[i*2] = c;
  fb[i*2 + 1] = ((bg & 0x0f) << 4) | (fg & 0x0f);
}

void fb_move_cursor(unsigned short pos) {
  outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
  outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  outb(FB_DATA_PORT, pos & 0x00FF);
}

// move cursor to next line
void fb_newline() {
  if (fb_y >= FB_HEIGHT-1)
    fb_shift_up();
  else
    fb_y++;

  fb_x = 0;
  fb_move_cursor(fb_x + (fb_y * FB_WIDTH));
}

void fb_fwd() {
  if (fb_x >= FB_WIDTH-1)
    fb_newline();
  else
    fb_x++;

  fb_move_cursor(fb_x + (fb_y * FB_WIDTH));
}

void fb_write(char *buf, unsigned int len) {
  unsigned int i;
  uint16_t pos;
  for (i=0; i<len; i++) {
    char c = buf[i];
    if (c == '\n' || c == '\r') {
      fb_newline();
    } else if (c == '\t') {
      fb_write_str(TAB);
    } else {
      pos = fb_x + (fb_y * FB_WIDTH);
      fb_write_cell(pos, c, FB_WHITE, FB_BLACK);
      fb_fwd();
    }
  }
}

void fb_write_str(char *buf) {
  fb_write(buf, strlen(buf));
}

void fb_clear() {
  fb_x = 0;
  fb_y = 0;

  int i;
  for (i=0; i<FB_WIDTH*FB_HEIGHT; i++) {
    fb_write_cell(i, ' ', FB_WHITE, FB_BLACK);
  }
  fb_move_cursor(0);
}

void fb_clr(uint8_t row) {
  size_t i;
  for (i=0; i<FB_WIDTH; i++) {
    fb_write_cell((row*FB_WIDTH)+i, ' ', FB_WHITE, FB_BLACK);
  }
}

void fb_shift_up() {
  uint16_t *fb = (uint16_t*)FRAMEBUFFER_ADDR;
  memmove(fb, fb+FB_WIDTH, FB_WIDTH*2*(FB_HEIGHT*2-1));
  fb_clr(FB_HEIGHT-1);
}

void fb_ht() {
  fb_y = 0;
  fb_clr(fb_y);
  fb_move_cursor(0);
}
