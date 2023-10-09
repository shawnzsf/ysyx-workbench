#include <common.h>

#define MAX_SIZE 16

typedef struct {
  word_t pc;
  uint32_t inst;
} Iringbuf;

Iringbuf Itracebuf[MAX_SIZE];
int inst_pos = 0;
bool full = false;

void trace_inst(word_t pc, uint32_t inst) {
  Itracebuf[inst_pos].pc = pc;
  Itracebuf[inst_pos].inst = inst;
  inst_pos = (inst_pos + 1) % MAX_SIZE;
  full = full || inst_pos == 0;
}

void display_inst() {
  if (!full && inst_pos == 0) return;
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);

  int end = inst_pos;
  int i = full ? inst_pos : 0;

  char buf[128]; 
  char *p;
  printf("Executed instructions near the bad trap\n");
  do {
    p = buf;
    p += sprintf(buf, "-- 0x%x: %08x      ", Itracebuf[i].pc, Itracebuf[i].inst);
    disassemble(p, sizeof(buf), Itracebuf[i].pc, (uint8_t *)&Itracebuf[i].inst, 4);
    puts(buf);
  } while ((i = (i + 1) % MAX_SIZE) != end);
}

