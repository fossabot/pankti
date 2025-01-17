/* vim: set fileencoding=utf-8 tabstop=4 shiftwidth=4 expandtab */

#include "include/debug.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <uchar.h>
#include <wchar.h>

#include "include/common.h"
#include "include/helper/comp.h"
#include "include/instruction.h"
#include "include/obj.h"
#include "include/utils.h"
#include "include/value.h"

// disassemble instructions set
void dissm_ins_chunk(Instruction *ins, const char32_t *name) {
#if defined(PANK_OS_WIN)
    char *name_c = c_to_c(name, strlen32(name));

    cp_println(L"----> %S <----", name_c);
    free(name_c);
#else
    cp_println(L"----> %ls <----", name);
#endif

    for (int off = 0; off < ins->len;) {
        off = dissm_ins(ins, off);
    }
}

// debug constant instruction
int const_ins(const char *name, Instruction *ins, int off) {
    uint8_t con = ins->code[off + 1];
#ifdef PANK_OS_WIN
    cp_print(L"%-16S %4d '", name, con);
#else
    cp_print(L"%-16s %4d '", name, con);
#endif
    print_val(ins->consts.values[con]);
    cp_print(L"'\n");
    return off + 2;
}

// debug instruction with no operands
// such as OP_ADD , OP_SUB etc etc.
int simple_ins(const char *name, int offset) {
#ifdef PANK_OS_WIN
    cp_println(L"%S", name);
#else
    cp_println(L"%s", name);
#endif
    return offset + 1;
}

// debug set/get type instructions
// OP_GET_LOCAL , OP_SET_LOCAL
int bt_ins(const char *name, Instruction *ins, int offset) {
    uint8_t slot = ins->code[offset + 1];
#ifdef PANK_OS_WIN
    cp_println(L"%-16S %4d", name, slot);
#else
    cp_println(L"%-16s %4d", name, slot);
#endif
    return offset + 2;
}

// debug jump instructions
// OP_JMP_IF_FALSE , OP_LOOP
// OP_JMP
int jmp_ins(const char *name, int sign, Instruction *ins, int offset) {
    uint16_t jmp = (uint16_t)(ins->code[offset + 1] << 8);
    jmp |= ins->code[offset + 2];
#ifdef PANK_OS_WIN
    cp_println(L"%-16S %4d -> %d\n", name, offset, offset + 3 + sign * jmp);
#else
    cp_println(L"%-16s %4d -> %d\n", name, offset, offset + 3 + sign * jmp);
#endif
    return offset + 3;
}

// disassemble single instruction
int dissm_ins(Instruction *ins, int offset) {
    wprintf(L"%04d ", offset);
    if (offset > 0 && ins->lines[offset] == ins->lines[offset - 1]) {
        wprintf(L"   | ");
    } else {
        wprintf(L"%4d ", ins->lines[offset]);
    }
    uint8_t is = ins->code[offset];
    switch (is) {
        case OP_RETURN:
            return simple_ins("OP_RETURN", offset);
        case OP_NEG:
            return simple_ins("OP_NEG", offset);
        case OP_IMPORT_NONAME:
            return const_ins("OP_IMPORT_NONAME", ins, offset);
        case OP_ADD:
            return simple_ins("OP_ADD", offset);
        case OP_SUB:
            return simple_ins("OP_SUB", offset);
        case OP_MUL:
            return simple_ins("OP_MUL", offset);
        case OP_DIV:
            return simple_ins("OP_DIV", offset);
        case OP_CONST:
            return const_ins("OP_CONST", ins, offset);
        case OP_NIL:
            return simple_ins("OP_NIL", offset);
        case OP_TRUE:
            return simple_ins("OP_TRUE", offset);
        case OP_FALSE:
            return simple_ins("OP_FALSE", offset);
        case OP_NOT:
            return simple_ins("OP_NOT", offset);
        case OP_EQ:
            return simple_ins("OP_EQ", offset);
        case OP_GT:
            return simple_ins("OP_GT", offset);
        case OP_LT:
            return simple_ins("OP_LT", offset);
        case OP_SHOW:
            return simple_ins("OP_SHOW", offset);
        case OP_POP:
            return simple_ins("OP_POP", offset);
        case OP_DEF_GLOB:
            return const_ins("OP_DEF_GLOB", ins, offset);
        case OP_GET_GLOB:
            return const_ins("OP_GET_GLOB", ins, offset);
        case OP_SET_GLOB:
            return const_ins("OP_SET_GLOB", ins, offset);
        case OP_GET_LOCAL:
            return bt_ins("OP_GET_LOCAL", ins, offset);
        case OP_SET_LOCAL:
            return bt_ins("OP_SET_LOCAL", ins, offset);
        case OP_CALL:
            return bt_ins("OP_CALL", ins, offset);
        case OP_ARRAY:
            return bt_ins("OP_ARRAY", ins, offset);
        case OP_JMP:
            return jmp_ins("OP_JMP", 1, ins, offset);
        case OP_JMP_IF_FALSE:
            return jmp_ins("OP_JMP_IF_FALSE", 1, ins, offset);
        case OP_LOOP:
            return jmp_ins("OP_LOOP", -1, ins, offset);
        case OP_GET_UP:
            return bt_ins("OP_GET_UP", ins, offset);
        case OP_SET_UP:
            return bt_ins("OP_SET_UP", ins, offset);
        case OP_CLS_UP:
            return simple_ins("OP_CLS_UPV", offset);
        case OP_GET_MOD_PROP:
            return const_ins("OP_GET_MOD_PROP", ins, offset);
        case OP_END_MOD:
            return simple_ins("OP_END_MOD", offset);
        case OP_ERR:
            return simple_ins("OP_ERR", offset);
        case OP_ARR_INDEX:
            return simple_ins("OP_ARR_INDEX", offset);
        case OP_HMAP:
            return bt_ins("OP_HMAP", ins, offset);
        case OP_CLOSURE:
            offset++;
            uint8_t con = ins->code[offset++];
#ifdef PANK_OS_WIN

            cp_print(L"%-16S %4d", "OP_CLOSURE", con);
#else
            cp_print(L"%-16s %4d", "OP_CLOSURE", con);
#endif
            print_val(ins->consts.values[con]);
            cp_println(L"");

            ObjFunc *func = get_as_func(ins->consts.values[con]);

            // read up values after closure declaration
            for (int x = 0; x < func->up_count; x++) {
                int is_local = ins->code[offset++];
                int index = ins->code[offset++];
#ifdef PANK_OS_WIN
                cp_println(L"%04d  |   -> %S %d", offset - 2,
                           is_local ? "local" : "upvalue", index);
#else
                cp_println(L"%04d  |   -> %s %d", offset - 2,
                           is_local ? "local" : "upvalue", index);
#endif
            }

            return offset;
        case OP_SUBSCRIPT_ASSIGN:
            return simple_ins("OP_SUBSCRIPT_ASSIGN", offset);
        default:
            cp_println(L"Unknown op %d", is);
            return offset + 1;
    }
}

// cppcheck-suppress unusedFunction
const char *print_opcode(Op op) {
    switch (op) {
        case OP_GTE:
            return "OP_GTE";
        case OP_LTE:
            return "OP_LTE";
        case OP_SET_MOD_PROP:
            return "OP_SET_MOD_PROP";
        case OP_RETURN:
            return "OP_RETURN";
        case OP_NEG:
            return "OP_NEG";
        case OP_IMPORT_NONAME:
            return "OP_IMPORT_NONAME";
        case OP_ADD:
            return "OP_ADD";
        case OP_SUB:
            return "OP_SUB";
        case OP_MUL:
            return "OP_MUL";
        case OP_DIV:
            return "OP_DIV";
        case OP_CONST:
            return "OP_CONST";
        case OP_NIL:
            return "OP_NIL";
        case OP_TRUE:
            return "OP_TRUE";
        case OP_FALSE:
            return "OP_FALSE";
        case OP_NOT:
            return "OP_NOT";
        case OP_EQ:
            return "OP_EQ";
        case OP_GT:
            return "OP_GT";
        case OP_LT:
            return "OP_LT";
        case OP_SHOW:
            return "OP_SHOW";
        case OP_POP:
            return "OP_POP";
        case OP_DEF_GLOB:
            return "OP_DEF_GLOB";
        case OP_GET_GLOB:
            return "OP_GET_GLOB";
        case OP_SET_GLOB:
            return "OP_SET_GLOB";
        case OP_GET_LOCAL:
            return "OP_GET_LOCAL";
        case OP_SET_LOCAL:
            return "OP_SET_LOCAL";
        case OP_CALL:
            return "OP_CALL";
        case OP_ARRAY:
            return "OP_ARRAY";
        case OP_JMP:
            return "OP_JMP";
        case OP_JMP_IF_FALSE:
            return "OP_JMP_IF_FALSE";
        case OP_LOOP:
            return "OP_LOOP";
        case OP_GET_UP:
            return "OP_GET_UP";
        case OP_SET_UP:
            return "OP_SET_UP";
        case OP_CLS_UP:
            return "OP_CLS_UPV";
        case OP_GET_MOD_PROP:
            return "OP_GET_MOD_PROP";
        case OP_END_MOD:
            return "OP_END_MOD";
        case OP_CLOSURE:
            return "OP_CLOSURE";
        case OP_ERR:
            return "OP_ERR";
        case OP_ARR_INDEX:
            return "OP_ARR_INDEX";
        case OP_HMAP:
            return "OP_HMAP";
        case OP_SUBSCRIPT_ASSIGN:
            return "OP_SUBSCRIPT_ASSIGN";
    }
    return "Unknown Opcode";
}
