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
static char *section_flag_selector(uint64_t flag)
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

// Return the correct char that corresponds to the flag
static char *program_flag_selector(uint64_t flag)
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
            case PF_R:
                res[index++] = 'R';
                break;
            case PF_W:
                res[index++] = 'W';
                break;
            case PF_X:
                res[index++] = 'E';
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

// Pretty print for sections headers
void pretty_print_section_header(ElfW(Shdr) *section, size_t number, section_info *section_info)
{
    if (str_sections_name == NULL)
    {
        err(1, "Cannot get sections names !");
    }

    puts("Sections Headers:");
    for (size_t i = 0; i < 10; i++)
    {
        auto_pad(section_attribute[i], PRINT_PAD);
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

        if (strcmp(name, ".dynstr") == 0)
        {
            section_info->str_dynamic_symbol_off = section[i].sh_offset;
        }
        if (strcmp(name, ".strtab") == 0)
        {
            section_info->str_symbol_off = section[i].sh_offset;
        }

        auto_pad(name, PRINT_PAD);
        auto_pad(xlat_get(sh_type, section[i].sh_type), PRINT_PAD);
        auto_pad_number((int)section[i].sh_addr, "%x", PRINT_PAD, 1);
        auto_pad_number((int)section[i].sh_offset, "%x", PRINT_PAD, 1);
        auto_pad_number((int)section[i].sh_size, "%x", PRINT_PAD, 1);
        auto_pad_number((int)section[i].sh_entsize, "%x", PRINT_PAD, 1);
        char *flag = section_flag_selector(section[i].sh_flags);
        auto_pad(flag, PRINT_PAD);
        auto_pad_number((int)section[i].sh_link, "%d", PRINT_PAD, 0);
        auto_pad_number((int)section[i].sh_info, "%d", PRINT_PAD, 0);
        auto_pad_number((int)section[i].sh_addralign, "%x", PRINT_PAD, 0);

        free(flag);
        putchar('\n');
    }
    puts(flag_section_keyword_infos);
}

// Pretty print for program headers
void pretty_print_program_header(ElfW(Phdr) *programs, size_t number)
{
    puts("Program Headers:");
    for (size_t i = 0; i < 8; i++)
    {
        auto_pad(program_attribute[i], PRINT_PAD);
    }
    putchar('\n');

    for (size_t i = 0; i < number; i++)
    {
        auto_pad(xlat_get(p_type, programs[i].p_type), PRINT_PAD);
        auto_pad_number((int)programs[i].p_offset, "%x", PRINT_PAD, 1);
        auto_pad_number((int)programs[i].p_vaddr, "%x", PRINT_PAD, 1);
        auto_pad_number((int)programs[i].p_paddr, "%x", PRINT_PAD, 1);
        auto_pad_number((int)programs[i].p_filesz, "%x", PRINT_PAD, 1);
        auto_pad_number((int)programs[i].p_memsz, "%x", PRINT_PAD, 1);
        char *flag = program_flag_selector(programs[i].p_flags);
        auto_pad(flag, PRINT_PAD);
        auto_pad_number((int)programs[i].p_align, "%x", PRINT_PAD, 0);

        free(flag);
        putchar('\n');
    }
    puts(flag_program_keyword_infos);
}

// Pretty print for symbol table
void pretty_print_symbol(ElfW(Sym) *symbol, size_t number, SYMBOL type)
{
    if (!dynamic_symbol_name || !symbol_name)
    {
        err(1, "Cannot get symbol names !");
    }

    if (type == STATIC)
    {
        printf("Symbol table '.symtab' contains %lu entries:\n\n", number);
    }
    else
    {
        printf("Symbol table '.dynsym' contains %lu entries:\n\n", number);
    }

    for (size_t i = 0; i < 8; i++)
    {
        auto_pad(dynamic_symbol_attribute[i], PRINT_PAD);
    }
    putchar('\n');
    for (size_t i = 0; i < number; i++)
    {
        auto_pad_number((int)i, "%i", PRINT_PAD, 0);
        auto_pad_number((int)symbol[i].st_value, "%i", PRINT_PAD, 1);
        auto_pad_number((int)symbol[i].st_size, "%i", PRINT_PAD, 0);
        auto_pad(xlat_get(dyn_sym_type, ELF64_ST_TYPE(symbol[i].st_info)), PRINT_PAD);
        auto_pad(xlat_get(dyn_sym_bind, ELF64_ST_BIND(symbol[i].st_info)), PRINT_PAD);
        auto_pad(xlat_get(dyn_sym_vis, ELF64_ST_VISIBILITY(symbol[i].st_other)), PRINT_PAD);
        const char *index_value = xlat_get(dyn_sym_index, symbol[i].st_shndx);
        if (index_value)
        {
            auto_pad(index_value, PRINT_PAD);
        }
        else
        {
            auto_pad_number((int)symbol[i].st_shndx, "%i", PRINT_PAD, 0);
        }

        const char *name = type == STATIC ? &symbol_name[symbol[i].st_name] : &dynamic_symbol_name[symbol[i].st_name];
        auto_pad(name, PRINT_PAD);
        putchar('\n');
    }
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
    // Get program header with the buffer address
    ElfW(Phdr) *program_header = (ElfW(Phdr) *)(buffer + elf_header->e_ehsize);
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
    putchar('\n');
    // Pretty print program headers
    pretty_print_program_header(program_header, elf_header->e_phnum);
    // Get the dynamic symbol table
    ElfW(Sym) *dynamic_symbol = (ElfW(Sym) *)(buffer + s_info.dynamic_symbol->sh_offset);
    ElfW(Sym) *symbol = (ElfW(Sym) *)(buffer + s_info.symbol->sh_offset);
    dynamic_symbol_name = buffer + s_info.str_dynamic_symbol_off;
    symbol_name = buffer + s_info.str_symbol_off;
    size_t number_dynamic_symbol = s_info.dynamic_symbol->sh_size / sizeof(ElfW(Sym));
    size_t number_symbol = s_info.symbol->sh_size / sizeof(ElfW(Sym));
    putchar('\n');
    // Pretty print dynamic symbol table
    pretty_print_symbol(dynamic_symbol, number_dynamic_symbol, DYNAMIC);
    putchar('\n');
    // Pretty print symbol table
    pretty_print_symbol(symbol, number_symbol, STATIC);

    // Free buffer memory
    free(buffer);

    return 0;
}
