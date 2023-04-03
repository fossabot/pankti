#ifndef cpank_utils_h
#define cpank_utils_h

#include <stdbool.h>
#include <uchar.h>
#include <wchar.h>

#define ANSI_COLOR_BLACK L"\x1b[30m"
#define ANSI_COLOR_RED L"\x1b[31m"
#define ANSI_COLOR_GREEN L"\x1b[32m"
#define ANSI_COLOR_YELLOW L"\x1b[33m"
#define ANSI_COLOR_BLUE L"\x1b[34m"
#define ANSI_COLOR_PURPLE L"\x1b[35m"
#define ANSI_COLOR_CYAN L"\x1b[36m"
#define ANSI_COLOR_WHITE L"\x1b[37m"
#define ANSI_COLOR_RESET L"\x1b[0m"

int copy_c16(char16_t *str, const char16_t *input, int len);

int strlen16(const char16_t *strarg);

char *c_to_c(const char16_t *input, int len);

bool str16cmp(const char16_t *str1, const char16_t *str2);

char16_t *chto16(char *input);
// check if `filepath` exists
bool does_file_exist(const char *filepath);

// print widechars to stdout without newline
void cp_print(const wchar_t *format, ...);

// print widechars to stdout with newline
void cp_println(const wchar_t *format, ...);

// print widechars to stderr without newline
void cp_err_print(const wchar_t *format, ...);

// print widechars to stderr without newline
void cp_err_println(const wchar_t *format, ...);

// print widechars to stdout without newline with colors
//  colorcode ->
//  			   r -> Red
//  			   b -> Blue
//  			   g -> Green
//  			   c -> Cyan
//  			   w -> White
//  			   p -> Purple
//  			   y -> Yellow
//  			   B -> Black
//  			   none -> default colors
void cp_color_print(wchar_t colorcode, const wchar_t *format, ...);

// print widechars to stdout with newline with colors
//  colorcode ->
//  			   r -> Red
//  			   b -> Blue
//  			   g -> Green
//  			   c -> Cyan
//  			   w -> White
//  			   p -> Purple
//  			   y -> Yellow
//  			   B -> Black
//  			   none -> default colors
void cp_color_println(wchar_t colorcode, const wchar_t *format, ...);

// print widechars to stderr without newline with colors
//  colorcode ->
//  			   r -> Red
//  			   b -> Blue
//  			   g -> Green
//  			   c -> Cyan
//  			   w -> White
//  			   p -> Purple
//  			   y -> Yellow
//  			   B -> Black
//  			   none -> default colors
void cp_err_color_print(wchar_t colorcode, const wchar_t *format, ...);

// print widechars to stdout with newline with colors
//  colorcode ->
//  			   r -> Red
//  			   b -> Blue
//  			   g -> Green
//  			   c -> Cyan
//  			   w -> White
//  			   p -> Purple
//  			   y -> Yellow
//  			   B -> Black
//  			   none -> default colors
void cp_err_color_println(wchar_t colorcode, const wchar_t *format, ...);

#endif
