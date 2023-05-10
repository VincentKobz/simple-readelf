#include "tools.h"
#include <err.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

size_t nb_relocatable_section = 0;
size_t max_relocatable_section = 0;

void auto_pad_number(long number, const char *format, size_t target_length, int is_address) {
    char number_str[16] = {0};
    // Convert input string to right number format
    if (sprintf(number_str, format, number) < 0) {
        errx(1, "Error during convert number to string !");
    }

    if (is_address) {
        char temp[16] = {0};
        size_t number_str_len = strlen(number_str);

        // Copy number_str to temp
        for (size_t i = 0; i < number_str_len; i++) {
            temp[i] = number_str[i];
        }
        // Pad with zeros
        for (size_t i = 0; i < 15; i++) {
            if (i < 15 - number_str_len)
                number_str[i] = '0';
            else
                number_str[i] = temp[i - 15 + number_str_len];
        }
    }
    // Pad with spaces
    auto_pad(number_str, target_length);
}

// Write on stdout the string and pad it with spaces to respect the target total length
void auto_pad(const char *string, size_t target_length) {
    if (!string) {
        string = "";
    }
    size_t i = 0;
    for (i = 0; i < strlen(string) && i < target_length - 1; i++) {
        putchar(string[i]);
    }

    for (; i < target_length; i++) {
        putchar(' ');
    }
}

// Pretty print for magic number
void pretty_print_magic(unsigned char *string) {
    for (size_t i = 0; i < EI_NIDENT; i++) {
        printf("%2.2x ", string[i]);
    }
    puts("");
}

// Pretty print header indent
void printer_indent(const char *title, const char *format, ...) {
    char string[128] = {0};
    va_list args;
    va_start(args, format);

    printf(INDENT);
    auto_pad(title, NB_INDENT);

    vsprintf(string, format, args);
    puts(string);
}

// Pretty print for elf head for data encoding
char *pretty_print_header_data(int data) {
    if (data == ELFDATA2LSB)
        return "2's complement, little-endian";
    else if (data == ELFDATA2MSB)
        return "2's complement, big-endian";
    else
        return "none";
}

// Pretty print for object file type
char *pretty_print_header_type(int type) {
    switch (type) {
        case ET_NONE:
            return "NONE (none)";
        case ET_REL:
            return "REL (Relocatable file)";
        case ET_EXEC:
            return "EXEC (Executable file)";
        case ET_DYN:
            return "DYN (Shared object file)";
        default:
            return "CORE (Core file)";
    }
}

// Pretty print for elf header OS ABI
char *pretty_print_header_osabi(int os_abi) {
    switch (os_abi) {
        case ELFOSABI_SYSV:
            return "UNIX System V ABI";
        case ELFOSABI_HPUX:
            return "HP-UX ABI";
        case ELFOSABI_NETBSD:
            return "NetBSD ABI";
        case ELFOSABI_LINUX:
            return "Linux ABI";
        case ELFOSABI_SOLARIS:
            return "Solaris ABI";
        case ELFOSABI_IRIX:
            return "IRIX ABI";
        case ELFOSABI_FREEBSD:
            return "FreeBSD ABI";
        case ELFOSABI_TRU64:
            return "TRU64 UNIX ABI";
        case ELFOSABI_ARM:
            return "ARM architecture ABI";
        default:
            return "Stand-alone (embedded) ABI";
    }
}

// Return the correct char that corresponds to the flag
char *section_flag_selector(uint64_t flag) {
    char *res = calloc(17, sizeof(char));

    if (!res) {
        errx(1, "Error during calloc !");
    }

    size_t index = 0;

    for (size_t i = 0; i < 16; i++) {
        uint64_t mask = 1 << i;
        uint64_t flag_mask = flag & mask;
        switch (flag_mask) {
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
char *program_flag_selector(uint64_t flag) {
    char *res = calloc(17, sizeof(char));

    if (!res) {
        errx(1, "Error during calloc !");
    }

    size_t index = 0;

    for (size_t i = 0; i < 16; i++) {
        uint64_t mask = 1 << i;
        uint64_t flag_mask = flag & mask;
        switch (flag_mask) {
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

ElfW(Shdr) * add_relocatable_section(ElfW(Shdr) * array, ElfW(Shdr) relocatable_section) {

    if (!array) {
        array = calloc(10, sizeof(ElfW(Shdr)));
        max_relocatable_section = 10;
        if (!array) {
            errx(1, "Error during calloc array of relocatable sections !");
        }
    }

    if (nb_relocatable_section == max_relocatable_section) {
        max_relocatable_section *= 2;
        ElfW(Shdr) *new_array = realloc(array, max_relocatable_section);

        if (!new_array) {
            free(array);
            errx(1, "Cannot realloc array of relocatable sections !");
        }
        array = new_array;
    }
    array[nb_relocatable_section++] = relocatable_section;

    return array;
}
