#ifndef SIMPLE_READELF_TOOLS_H
#define SIMPLE_READELF_TOOLS_H
#include <stdio.h>

void auto_pad(const char *string, size_t target_length);
void auto_pad_number(int number, const char *format, size_t target_length, int is_address);

#endif //SIMPLE_READELF_TOOLS_H
