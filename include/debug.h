#ifndef cpank_debug_h
#define cpank_debug_h

#include "instruction.h"
void dissm_ins_chunk(Instruction *ins, const wchar_t *name);
int dissm_ins(Instruction *ins, int offset);
wchar_t * get_single_ins(uint8_t ins);
#endif
