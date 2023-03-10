#ifndef cpank_obj_h
#define cpank_obj_h

#include "common.h"
#include "value.h"
#include <stdbool.h>
#include <wchar.h>

typedef enum {
  OBJ_STR,
} ObjType;

struct Obj {
  ObjType type;
};

struct ObjString {
  Obj obj;
  int len;
  wchar_t *chars;
};

ObjType get_obj_type(Value val);
bool is_obj_type(Value val, ObjType ot);
bool is_str_obj(Value val);
ObjString *get_as_string(Value val);
wchar_t *get_as_native_string(Value val);
ObjString *copy_string(wchar_t *chars, int len);
void print_obj(Value val);

#endif
