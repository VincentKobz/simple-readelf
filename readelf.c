#include <stdio.h>
#include <elf.h>
#include <link.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <stdarg.h>
#include "readelf.h"

// Useful macro
#define INDENT "  "
#define NB_INDENT 30

// Find correct name parameter in xlat
const char *xlat_get(struct xlat *xlat_arr, size_t val)
{
    for (; xlat_arr->string; xlat_arr++)
    {
        if (xlat_arr->value == val)
            return xlat_arr->string;
    }
    return NULL;
}

// Pretty print for magic number
void pretty_print_magic(unsigned char *string)
{
    for (size_t i = 0; i < EI_NIDENT; i++)
    {
        printf ("%2.2x ", string[i]);
    }
    puts("");
}

// Pretty print for elf head for data encoding
static char *pretty_print_header_data(int data)
{
    if (data == ELFDATA2LSB)
        return "2's complement, little-endian";
    else if (data == ELFDATA2MSB)
        return "2's complement, big-endian";
    else
        return "none";
}

// Pretty print for elf header OS ABI
static char *pretty_print_header_osabi(int os_abi)
{
    switch (os_abi)
    {
        case ELFOSABI_SYSV: return "UNIX System V ABI";
        case ELFOSABI_HPUX: return "HP-UX ABI";
        case ELFOSABI_NETBSD: return "NetBSD ABI";
        case ELFOSABI_LINUX: return "Linux ABI";
        case ELFOSABI_SOLARIS: return "Solaris ABI";
        case ELFOSABI_IRIX: return "IRIX ABI";
        case ELFOSABI_FREEBSD: return "FreeBSD ABI";
        case ELFOSABI_TRU64: return "TRU64 UNIX ABI";
        case ELFOSABI_ARM: return "ARM architecture ABI";
        default: return "Stand-alone (embedded) ABI";
    }
}

// Pretty print for object file type
static char *pretty_print_header_type(int type)
{
    switch (type)
    {
        case ET_NONE: return "NONE (none)";
        case ET_REL: return "REL (Relocatable file)";
        case ET_EXEC: return "EXEC (Executable file)";
        case ET_DYN: return "DYN (Shared object file)";
        default: return "CORE (Core file)";
    }
}

// Pretty print header indent
static void printer_indent(const char *title, const char *format, ...)
{
    char string[128] = {0};
    va_list args;
    va_start(args, format);

    printf(INDENT "%s", title);
    for (size_t i = strlen(title); i < NB_INDENT; i++)
    {
        putchar(' ');
    }

    vsprintf(string, format, args);
    puts(string);
}

// Pretty print ELF header
void prettry_print_header(ElfW(Ehdr) *header)
{
    // Header title
    puts("ELF Header:");

    // Magic print
    printf(INDENT "Magic: ");
    pretty_print_magic(header->e_ident);

    // Header info
    printer_indent("Data:", "%s", pretty_print_header_data(header->e_ident[EI_DATA]));
    printer_indent("Version:", "%d (current)", header->e_ident[EI_VERSION]);
    printer_indent("OS/ABI:", "%s", pretty_print_header_osabi(header->e_ident[EI_OSABI]));
    printer_indent("ABI Version:", "%d", header->e_ident[EI_ABIVERSION]);
    printer_indent("Type:", "%s", pretty_print_header_type(header->e_type));
    printer_indent("Machine:", "%s", xlat_get(e_machine, header->e_machine));
    printer_indent("Version:", "0x%1x", header->e_version);
    printer_indent("Entry point address:", "0x%ld", header->e_entry);
    printer_indent("Start of program headers:", "%ld (bytes into file)", header->e_phoff);
    printer_indent("Start of section headers:", "%ld (bytes into file)", header->e_shoff);
    printer_indent("Flags:", "0x%1x", header->e_flags);
    printer_indent("Size of this header:", "%d (bytes)", header->e_ehsize);
    printer_indent("Size of program headers:", "%d (bytes)", header->e_phentsize);
    printer_indent("Number of program headers:", "%d", header->e_phnum);
    printer_indent("Size of section headers:", "%d (bytes)", header->e_shentsize);
    printer_indent("Number of section headers:", "%d", header->e_shnum);
    printer_indent("Section header string table index:", "%d", header->e_shstrndx);
}

// Process input file
char *open_wrapper(char *filename)
{
    size_t buffer_size = 1000;
    size_t counter = 0;
    char *buffer = calloc(1000, sizeof(char));

    if (!buffer)
        err(1, "Cannot malloc buffer !");

    FILE *file = fopen(filename, "r");

    if (!file)
        err(1, "Cannot open file !");

    for (;;)
    {
        if (counter == buffer_size - 1)
        {
            buffer_size *= 2;
            buffer = realloc(buffer, buffer_size);

            if (!buffer)
                err(1, "Cannot realloc buffer !");
        }

        buffer[counter++] = (char) fgetc(file);

        if (feof(file))
            break;
    }
    return buffer;
}

// Main function
int main(int argc, char **argv)
{
    // Check function number of argument
    if (argc != 2)
    {
        puts("Usage: ./simple-readelf <filename>");
        return 1;
    }

    // Copy content file inside a buffer
    char *buffer = open_wrapper(argv[1]);

    // Get the elf header with the buffer address
    ElfW(Ehdr) *elf_header = (ElfW(Ehdr) *)buffer;

    // Define symbole table offset
    // ElfW(Shdr) *sym_s = NULL;
    // ElfW(Shdr) *dyn_sym_s = NULL;

    // String tables offset
    // size_t str_dyn_offset = 0;
    // size_t str_offset = 0;

    // Prettry print header
    prettry_print_header(elf_header);
    free(buffer);

    return 0;
}
