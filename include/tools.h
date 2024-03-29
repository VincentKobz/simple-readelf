#ifndef SIMPLE_READELF_TOOLS_H
#define SIMPLE_READELF_TOOLS_H
#include <elf.h>
#include <stdio.h>

// Macros
#define INDENT "  "
#define NB_INDENT 35

// Functions declaration
void auto_pad(const char *string, size_t target_length);
void auto_pad_number(int number, const char *format, size_t target_length, int is_address);
void pretty_print_magic(unsigned char *string);
void printer_indent(const char *title, const char *format, ...);
char *pretty_print_header_data(int data);
char *pretty_print_header_type(int type);
char *pretty_print_header_osabi(int os_abi);
char *section_flag_selector(uint64_t flag);
char *program_flag_selector(uint64_t flag);

#endif//SIMPLE_READELF_TOOLS_H
