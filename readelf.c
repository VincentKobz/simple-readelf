#include <stdio.h>
#include <elf.h>
#include <link.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <stdarg.h>
#include "readelf.h"
#include "tools.h"


// Find correct name parameter in xlat
const char *xlat_get(xlat *xlat_arr, size_t val)
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

// Return the correct char that corresponds to the flag
static char *flag_selector(uint64_t flag)
{
    char *res = calloc(17, sizeof(char));

    if (!res)
    {
        err(1, "Error during calloc !");
    }

    size_t index = 0;

    for (size_t i = 0; i < 16; i++)
    {
        uint64_t mask = 1 << i;
        uint64_t flag_mask = flag & mask;
        switch(flag_mask)
        {
            case SHF_WRITE:
                res[index++] = 'W';
                break;
            case SHF_ALLOC:
                res[index++] = 'A';
                break;
            case SHF_EXECINSTR:
                res[index++] = 'X';
                break;
            case SHF_MERGE:
                res[index++] = 'M';
                break;
            case SHF_STRINGS:
                res[index++] = 'S';
                break;
            case SHF_INFO_LINK:
                res[index++] = 'I';
                break;
            case SHF_LINK_ORDER:
                res[index++] = 'L';
                break;
            case SHF_OS_NONCONFORMING:
                res[index++] = 'O';
                break;
            case SHF_GROUP:
                res[index++] = 'G';
                break;
            case SHF_TLS:
                res[index++] = 'T';
                break;
            case SHF_EXCLUDE:
                res[index++] = 'E';
                break;
            case SHF_COMPRESSED:
                res[index++] = 'C';
                break;
            default:
                break;
        }
    }
    return res;
}

// Pretty print header indent
static void printer_indent(const char *title, const char *format, ...)
{
    char string[128] = {0};
    va_list args;
    va_start(args, format);

    printf(INDENT);
    auto_pad(title, NB_INDENT);

    vsprintf(string, format, args);
    puts(string);
}

// Pretty print ELF header
void pretty_print_header(ElfW(Ehdr) *header)
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

void pretty_print_section_header(ElfW(Shdr) *section, size_t number, section_info *section_info)
{
    if (str_sections_name == NULL)
    {
        err(1, "Cannot get sections names !");
    }

    puts("Sections Headers:");
    for (size_t i = 0; i < 10; i++)
    {
        auto_pad(section_attribute[i], SECTION_PAD);
    }
    putchar('\n');

    for (size_t i = 0; i < number; i++)
    {
        char *name = &str_sections_name[section[i].sh_name];
        if (section[i].sh_type == SHT_SYMTAB)
        {
            section_info->symbol = &section[i];
        }
        else if (section[i].sh_type == SHT_DYNSYM)
        {
            section_info->dynamic_symbol = &section[i];
        }
        auto_pad(name, SECTION_PAD);
        auto_pad(xlat_get(sh_type, section[i].sh_type), SECTION_PAD);
        auto_pad_number((int)section[i].sh_addr, "%x", SECTION_PAD, 1);
        auto_pad_number((int)section[i].sh_offset, "%x", SECTION_PAD, 1);
        auto_pad_number((int)section[i].sh_size, "%x", SECTION_PAD, 1);
        auto_pad_number((int)section[i].sh_entsize, "%x", SECTION_PAD, 1);
        char *flag = flag_selector(section[i].sh_flags);
        auto_pad(flag, SECTION_PAD);
        auto_pad_number((int)section[i].sh_link, "%d", SECTION_PAD, 0);
        auto_pad_number((int)section[i].sh_info, "%d", SECTION_PAD, 0);
        auto_pad_number((int)section[i].sh_addralign, "%x", SECTION_PAD, 0);

        free(flag);
        putchar('\n');
    }
    puts(flag_keyword_infos);
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
            char* new_buffer = realloc(buffer, buffer_size);

            if (!new_buffer)
            {
                free(buffer);
                err(1, "Cannot realloc buffer !");
            }
            buffer = new_buffer;
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
    // Get sections header with the buffer address
    ElfW(Shdr) *sections_header = (ElfW(Shdr *))(buffer + elf_header->e_shoff);
    // Get the section that hosts the section header names
    ElfW(Shdr) str_section_name_s = sections_header[elf_header->e_shstrndx];

    size_t nb_sections = elf_header->e_shnum;
    section_info s_info;

    // Assign global variable for sections header names
    str_sections_name = buffer + str_section_name_s.sh_offset;

    // Pretty print ELF header
    pretty_print_header(elf_header);
    putchar('\n');
    // Pretty print sections headers
    pretty_print_section_header(sections_header, nb_sections, &s_info);
    // Free buffer memory
    free(buffer);

    return 0;
}
