#ifndef READELF_H
#define READELF_H

#include <elf.h>

// Macros
#define XLAT(val) {(size_t)(val), #val}
#define XLAT_END {0, 0}
#define INDENT "  "
#define NB_INDENT 35
#define SECTION_PAD 18

// Functions declaration
char *str_sections_name = NULL;
const char *section_attribute[10] = {"Name", "Type", "Address", "Offset", "Size", "EntSize", "Flags", "Links", "Info", "Align"};

// Structures declaration
typedef struct {
    ElfW(Shdr) *dynamic_symbol;
    ElfW(Shdr) *symbol;
} section_info;

typedef struct {
    size_t value;
    const char *string;
} xlat;

// Xlat arrays

// Machines types
xlat e_machine[] = {
    XLAT(EM_M32),
    XLAT(EM_SPARC),
    XLAT(EM_386),
    XLAT(EM_68K),
    XLAT(EM_88K),
    XLAT(EM_860),
    XLAT(EM_MIPS),
    XLAT(EM_PARISC),
    XLAT(EM_SPARC32PLUS),
    XLAT(EM_PPC),
    XLAT(EM_PPC64),
    XLAT(EM_S390),
    XLAT(EM_ARM),
    XLAT(EM_SH),
    XLAT(EM_SPARCV9),
    XLAT(EM_X86_64),
    XLAT(EM_IA_64),
    XLAT_END
};

// Sections types
xlat sh_type[] = {
        XLAT(SHT_NULL),
        XLAT(SHT_PROGBITS),
        XLAT(SHT_SYMTAB),
        XLAT(SHT_STRTAB),
        XLAT(SHT_RELA),
        XLAT(SHT_HASH),
        XLAT(SHT_DYNAMIC),
        XLAT(SHT_NOTE),
        XLAT(SHT_NOBITS),
        XLAT(SHT_REL),
        XLAT(SHT_SHLIB),
        XLAT(SHT_DYNSYM),
        XLAT(SHT_NOTE),
        XLAT(SHT_GNU_HASH),
        XLAT(SHT_GNU_versym),
        XLAT(SHT_GNU_verneed),
        XLAT(SHT_INIT_ARRAY),
        XLAT(SHT_FINI_ARRAY),
        XLAT_END
};

xlat sh_flags[] = {
        XLAT(SHF_WRITE),
        XLAT(SHF_ALLOC),
        XLAT(SHF_EXECINSTR),
        XLAT(SHF_MERGE),
        XLAT(SHF_STRINGS),
        XLAT(SHF_INFO_LINK),
        XLAT(SHF_LINK_ORDER),
        XLAT(SHF_OS_NONCONFORMING),
        XLAT(SHF_GROUP),
        XLAT(SHF_TLS),
        XLAT(SHF_EXCLUDE),
        XLAT(SHF_COMPRESSED),
};

#endif //READELF_H
