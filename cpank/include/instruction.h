/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#ifndef cpank_instruction_h
#define cpank_instruction_h

#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "value.h"

typedef enum {
    OP_RETURN,
    OP_CONST,
    OP_NEG,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_NOT,
    OP_EQ,
    OP_GT,
    OP_GTE,
    OP_LT,
    OP_LTE,
    OP_SHOW,
    OP_POP,
    OP_DEF_GLOB,
    OP_SET_GLOB,
    OP_GET_GLOB,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_JMP_IF_FALSE,
    OP_JMP,
    OP_LOOP,
    OP_CALL,
    OP_CLOSURE,
    OP_GET_UP,
    OP_SET_UP,
    OP_CLS_UP,
    OP_IMPORT_NONAME,
    OP_SET_MOD_PROP,
    OP_GET_MOD_PROP,
    OP_END_MOD,
    OP_ERR,
    OP_ARRAY,
    OP_HMAP,
    OP_ARR_INDEX,

} Op;

typedef struct {
    int len;
    int cap;
    uint8_t *code;
    int *lines;
    Valarr consts;

} Instruction;

void init_instruction(Instruction *ins);
void write_ins(PankVm *vm, Instruction *ins, uint8_t bt, int line);
void free_ins(PankVm *vm, Instruction *ins);
int add_const(PankVm *vm, Instruction *ins, Value val);
bool make_changes_for_mod(Instruction *ins);
#endif
