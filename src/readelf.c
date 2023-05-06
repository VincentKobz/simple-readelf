#include "readelf.h"
#include "tools.h"
#include <elf.h>
#include <err.h>
#include <getopt.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Find correct name parameter in xlat
const char *xlat_get(xlat *xlat_arr, size_t val) {
    for (; xlat_arr->string; xlat_arr++) {
        if (xlat_arr->value == val)
            return xlat_arr->string;
    }
    return NULL;
}

// Pretty print ELF header
static void pretty_print_header(ElfW(Ehdr) * header) {
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
    putchar('\n');
}

// Pretty print for sections headers
static void pretty_print_section_header(ElfW(Shdr) * section, size_t number, section_info *section_info) {
    // Check presence of program headers
    if (number == 0) {
        printf("%s\n", no_section_headers);
        return;
    }

    if (str_sections_name == NULL) {
        err(1, "Cannot get sections names !");
    }

    if (options == ALL || options == SECTION_HEADER) {
        puts("Sections Headers:");
        for (size_t i = 0; i < 10; i++) {
            auto_pad(section_attribute[i], PRINT_PAD);
        }
        putchar('\n');
    }

    for (size_t i = 0; i < number; i++) {
        char *name = &str_sections_name[section[i].sh_name];
        if (section[i].sh_type == SHT_SYMTAB) {
            section_info->symbol = &section[i];
        } else if (section[i].sh_type == SHT_DYNSYM) {
            section_info->dynamic_symbol = &section[i];
        }

        if (strcmp(name, ".dynstr") == 0) {
            section_info->str_dynamic_symbol_off = section[i].sh_offset;
        }
        if (strcmp(name, ".strtab") == 0) {
            section_info->str_symbol_off = section[i].sh_offset;
        }

        if (options == ALL || options == SECTION_HEADER) {
            auto_pad(name, PRINT_PAD);
            auto_pad(xlat_get(sh_type, section[i].sh_type), PRINT_PAD);
            auto_pad_number((int) section[i].sh_addr, "%x", PRINT_PAD, 1);
            auto_pad_number((int) section[i].sh_offset, "%x", PRINT_PAD, 1);
            auto_pad_number((int) section[i].sh_size, "%x", PRINT_PAD, 1);
            auto_pad_number((int) section[i].sh_entsize, "%x", PRINT_PAD, 1);
            char *flag = section_flag_selector(section[i].sh_flags);
            auto_pad(flag, PRINT_PAD);
            auto_pad_number((int) section[i].sh_link, "%d", PRINT_PAD, 0);
            auto_pad_number((int) section[i].sh_info, "%d", PRINT_PAD, 0);
            auto_pad_number((int) section[i].sh_addralign, "%x", PRINT_PAD, 0);

            free(flag);
            putchar('\n');
        }
    }
    if (options == ALL || options == SECTION_HEADER) {
        puts(flag_section_keyword_infos);
        putchar('\n');
    }
}

// Pretty print for program headers
static void pretty_print_program_header(ElfW(Phdr) * programs, size_t number) {
    // Check presence of program headers
    if (number == 0) {
        printf("%s\n", no_program_headers);
        return;
    }
    puts("Program Headers:");
    for (size_t i = 0; i < 8; i++) {
        auto_pad(program_attribute[i], PRINT_PAD);
    }
    putchar('\n');

    for (size_t i = 0; i < number; i++) {
        auto_pad(xlat_get(p_type, programs[i].p_type), PRINT_PAD);
        auto_pad_number((int) programs[i].p_offset, "%x", PRINT_PAD, 1);
        auto_pad_number((int) programs[i].p_vaddr, "%x", PRINT_PAD, 1);
        auto_pad_number((int) programs[i].p_paddr, "%x", PRINT_PAD, 1);
        auto_pad_number((int) programs[i].p_filesz, "%x", PRINT_PAD, 1);
        auto_pad_number((int) programs[i].p_memsz, "%x", PRINT_PAD, 1);
        char *flag = program_flag_selector(programs[i].p_flags);
        auto_pad(flag, PRINT_PAD);
        auto_pad_number((int) programs[i].p_align, "%x", PRINT_PAD, 0);

        free(flag);
        putchar('\n');
    }
    puts(flag_program_keyword_infos);
    putchar('\n');
}

// Pretty print for symbol table
static void pretty_print_symbol(ElfW(Sym) * symbol, size_t number, SYMBOL type) {
    if (type == DYNAMIC && !dynamic_symbol_name) {
        err(1, "Cannot get dynamic symbol names !");
    }
    if (type == STATIC && !symbol_name) {
        err(1, "Cannot get symbol names !");
    }

    if (type == STATIC) {
        printf("Symbol table '.symtab' contains %lu entries:\n\n", number);
    } else {
        printf("Symbol table '.dynsym' contains %lu entries:\n\n", number);
    }

    for (size_t i = 0; i < 8; i++) {
        auto_pad(dynamic_symbol_attribute[i], PRINT_PAD);
    }
    putchar('\n');
    for (size_t i = 0; i < number; i++) {
        auto_pad_number((int) i, "%i", PRINT_PAD, 0);
        auto_pad_number((int) symbol[i].st_value, "%i", PRINT_PAD, 1);
        auto_pad_number((int) symbol[i].st_size, "%i", PRINT_PAD, 0);
        auto_pad(xlat_get(dyn_sym_type, ELF64_ST_TYPE(symbol[i].st_info)), PRINT_PAD);
        auto_pad(xlat_get(dyn_sym_bind, ELF64_ST_BIND(symbol[i].st_info)), PRINT_PAD);
        auto_pad(xlat_get(dyn_sym_vis, ELF64_ST_VISIBILITY(symbol[i].st_other)), PRINT_PAD);
        const char *index_value = xlat_get(dyn_sym_index, symbol[i].st_shndx);
        if (index_value) {
            auto_pad(index_value, PRINT_PAD);
        } else {
            auto_pad_number((int) symbol[i].st_shndx, "%i", PRINT_PAD, 0);
        }

        const char *name = type == STATIC ? &symbol_name[symbol[i].st_name] : &dynamic_symbol_name[symbol[i].st_name];
        auto_pad(name, PRINT_PAD);
        putchar('\n');
    }
    putchar('\n');
}

// Process input file
static char *open_wrapper(char *filename) {
    size_t buffer_size = 1000;
    size_t counter = 0;
    char *buffer = calloc(1000, sizeof(char));

    if (!buffer)
        err(1, "Cannot malloc buffer !");

    FILE *file = fopen(filename, "r");

    if (!file)
        err(1, "Cannot open file !");

    for (;;) {
        if (counter == buffer_size - 1) {
            buffer_size *= 2;
            char *new_buffer = realloc(buffer, buffer_size);

            if (!new_buffer) {
                free(buffer);
                err(1, "Cannot realloc buffer !");
            }
            buffer = new_buffer;
        }
        buffer[counter++] = (char) fgetc(file);

        if (feof(file))
            break;
    }
    fclose(file);
    return buffer;
}

static char *parse_options(int argc, char **argv) {
    char *filename = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "a:h:P:S:s:d:")) != -1) {
        switch (opt) {
            case 'a':
                options = ALL;
                filename = optarg;
                break;
            case 'h':
                options = HEADER;
                filename = optarg;
                break;
            case 'P':
                options = PROGRAM_HEADER;
                filename = optarg;
                break;
            case 'S':
                options = SECTION_HEADER;
                filename = optarg;
                break;
            case 's':
                options = STATIC_SYMBOL;
                filename = optarg;
                break;
            case 'd':
                options = DYNAMIC_SYMBOL;
                filename = optarg;
                break;
            default:
                errx(1, "Usage: ./simple-readelf [-a -h -P -S -s -d] <filename>");
        }
    }
    if (optind != 3) {
        errx(1, "Usage: ./simple-readelf [-a -h -P -S -s -d] <filename>");
    }
    return filename;
}

// Main function
int main(int argc, char **argv) {
    // Parse command line options
    char *filename = parse_options(argc, argv);
    // Copy content file inside a buffer
    char *buffer = open_wrapper(filename);

    // Get the elf header with the buffer address
    ElfW(Ehdr) *elf_header = (ElfW(Ehdr) *) buffer;
    // Get sections header with the buffer address
    ElfW(Shdr) *sections_header = (ElfW(Shdr *))(buffer + elf_header->e_shoff);
    // Get program header with the buffer address
    ElfW(Phdr) *program_header = (ElfW(Phdr) *) (buffer + elf_header->e_ehsize);
    // Get the section that hosts the section header names
    ElfW(Shdr) str_section_name_s = sections_header[elf_header->e_shstrndx];

    size_t nb_sections = elf_header->e_shnum;
    section_info s_info = {.symbol = NULL, .dynamic_symbol = NULL, .str_symbol_off = 0, .str_dynamic_symbol_off = 0};

    // Pretty print ELF header
    if (options == ALL || options == HEADER) {
        pretty_print_header(elf_header);
    }
    // Pretty print sections headers
    if (options != PROGRAM_HEADER) {
        // Assign global variable for sections header names
        str_sections_name = buffer + str_section_name_s.sh_offset;
        pretty_print_section_header(sections_header, nb_sections, &s_info);
    }
    // Pretty print program headers
    if (options == ALL || options == PROGRAM_HEADER) {
        pretty_print_program_header(program_header, elf_header->e_phnum);
    }
    // Pretty print dynamic symbol table
    if (options == ALL || options == DYNAMIC_SYMBOL) {
        if (s_info.dynamic_symbol) {
            size_t number_dynamic_symbol = s_info.dynamic_symbol->sh_size / sizeof(ElfW(Sym));
            dynamic_symbol_name = buffer + s_info.str_dynamic_symbol_off;

            // Get the dynamic symbol table
            ElfW(Sym) *dynamic_symbol = (ElfW(Sym) *) (buffer + s_info.dynamic_symbol->sh_offset);

            pretty_print_symbol(dynamic_symbol, number_dynamic_symbol, DYNAMIC);
        } else {
            printf("%s\n", no_dynamic_section);
        }
    }
    // Pretty print symbol table
    if (options == ALL || options == STATIC_SYMBOL) {
        if (s_info.symbol) {
            size_t number_symbol = s_info.symbol->sh_size / sizeof(ElfW(Sym));
            symbol_name = buffer + s_info.str_symbol_off;

            // Get the symbol table
            ElfW(Sym) *symbol = (ElfW(Sym) *) (buffer + s_info.symbol->sh_offset);

            pretty_print_symbol(symbol, number_symbol, STATIC);
        } else {
            printf("%s\n", no_symbol_section);
        }
    }
    // Free buffer memory
    free(buffer);

    return 0;
}
