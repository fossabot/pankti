#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <uchar.h>

#include "../include/helper/os.h"
#include "../include/openfile.h"
#include "../include/stdlib.h"
#include "../include/utils.h"

#define _NUM_TO_STR(x) #x
#define NUMTOSTR(x)    _NUM_TO_STR(x)

#define no_android(funcname)                                                   \
 if (get_os_code() == OS_ANDROID_CODE) {                                       \
  return make_error(vm,                                                        \
                    U"file function " funcname " do not work on android yet"); \
 }

bool check_is_file(char* path) {
#ifdef PANK_OS_WINDOWS
    struct _stat pbuf;
 #define S_ISREG _S_IFREG

    _stat(path, &pbuf);
#else
    struct stat pbuf;
    stat(path, &pbuf);
#endif
    return S_ISREG(pbuf.st_mode);
}

bool check_is_dir(char* path) {
#ifdef PANK_OS_WINDOWS
    struct _stat pbuf;
 #define S_ISDIR _S_IFDIR

    _stat(path, &pbuf);
#else
    struct stat pbuf;
    stat(path, &pbuf);
#endif
    return S_ISDIR(pbuf.st_mode);
}

Value _file_exists(PankVm* vm, int argc, Value* args) {
    //    char argstr[100];
    //    sprintf(argstr, "%d", argc);
    no_android("exists(path)");
    check_argc_count("exists(path)", 1, argc);
    /*if (argc != 1) {

        return make_error(vm, U"exists(a) only takes single argument");
    }*/
    if (!is_str_obj(args[0])) {
        return make_error(vm, U"argument to exists(a) must be a string");
    }

    ObjString* str = (ObjString*)get_as_obj(args[0]);
    char* fp = c_to_c(str->chars, str->len);

    if (does_file_exist(fp)) {
        free(fp);
        return make_bool(true);
    } else {
        free(fp);
        return make_bool(false);
    }
}

Value _file_read_file_as_string(PankVm* vm, int argc, Value* args) {
    no_android("readfile(path)");
    check_argc_count("readile(path)", 1, argc);

    if (!is_str_obj(args[0])) {
        return make_error(
            vm, U"argument to readfile(path <- string) must be a string");
    }
    ObjString* str = (ObjString*)get_as_obj(args[0]);

    WSrcfile fws = wread_file(str->chars);

    if (fws.errcode != 0) {
        return make_str(vm, U"");
    }

    Value result = make_str(vm, fws.source);
    free(fws.source);
    return result;
}

Value _file_is_file(PankVm* vm, int argc, Value* args) {
    no_android("isfile(path)");
    check_argc_count("isfile(path)", 1, argc);

    if (!is_str_obj(args[0])) {
        return make_error(
            vm, U"argument to isfile(path <- string) must be a string");
    }

    ObjString* wpath = (ObjString*)get_as_obj(args[0]);
    char* path = c_to_c(wpath->chars, wpath->len);

    bool result = check_is_file(path);
    free(path);
    return make_bool(result);
}

Value _file_is_dir(PankVm* vm, int argc, Value* args) {
    no_android("isdir(path)");
    check_argc_count("isdir(path)", 1, argc);

    ObjString* wpath = (ObjString*)get_as_obj(args[0]);
    char* path = c_to_c(wpath->chars, wpath->len);

    bool result = check_is_dir(path);
    free(path);
    return make_bool(result);
}

Value _file_create_empty_file(PankVm* vm, int argc, Value* args) {
    no_android("create_empty(path)");
    check_argc_count("create_empty(path)", 1, argc);
    if (!is_str_obj(args[0])) {
        return make_error(
            vm, U"argument to create_empty(path <- string) must be a string");
    }

    ObjString* wpath = (ObjString*)get_as_obj(args[0]);
    char* path = c_to_c(wpath->chars, wpath->len);
    bool result = true;
    FILE* fp;
    fp = fopen(path, "w");
    if (fp == NULL) {
        result = false;
    }
    fclose(fp);
    free(path);
    return make_bool(result);
}

Value _file_rename(PankVm* vm, int argc, Value* args) {
    no_android("rename(old , new)");
    check_argc_count("rename(old , new)", 2, argc);

    if (!is_str_obj(args[0]) || !is_str_obj(args[1])) {
        return make_error(vm,
                          U"argument to rename(old <- string , new <- string) "
                          U"must be a string");
    }

    ObjString* wpath_old = (ObjString*)get_as_obj(args[0]);
    char* old_path = c_to_c(wpath_old->chars, wpath_old->len);
    ObjString* wpath_new = (ObjString*)get_as_obj(args[1]);
    char* new_path = c_to_c(wpath_new->chars, wpath_new->len);
    bool result = true;
    if (rename(old_path, new_path) != 0) {
        result = false;
    }
    free(old_path);
    free(new_path);
    return make_bool(result);
}

Value _file_delete(PankVm* vm, int argc, Value* args) {
    no_android("delete(path)");
    check_argc_count("delete(path)", 1, argc);

    if (!is_str_obj(args[0])) {
        return make_error(
            vm, U"argument to delete(path <- string) must be a string");
    }

    ObjString* wpath = (ObjString*)get_as_obj(args[0]);
    char* path = c_to_c(wpath->chars, wpath->len);
    bool result = true;
    if (remove(path) != 0) {
        result = false;
    }
    free(path);
    return make_bool(result);
}

void push_stdlib_file(PankVm* vm) {
    SL sls[] = {
        msl(U"exists", _file_exists),
        msl(U"readfile", _file_read_file_as_string),
        msl(U"isfile", _file_is_file),
        msl(U"isdir", _file_is_dir),
        msl(U"create_empty", _file_create_empty_file),
        msl(U"rename", _file_rename),
        msl(U"delete", _file_delete),
    };

    _push_stdlib(vm, U"file", sls, 7);
}