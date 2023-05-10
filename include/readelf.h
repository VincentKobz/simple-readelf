#ifndef READELF_H
#define READELF_H

#include <elf.h>
#include <link.h>
#include <stddef.h>

// Macros
#define XLAT(val) \
    { (size_t)(val), #val }
#define XLAT_END \
    { 0, 0 }
#define PRINT_PAD 18

// Global variables declaration
char *str_sections_name = NULL;
char *dynamic_symbol_name = NULL;
char *symbol_name = NULL;
ElfW(Shdr) *relocatable_sections = NULL;
static const char *section_attribute[10] = {"Name", "Type", "Address", "Offset", "Size", "EntSize", "Flags", "Links", "Info", "Align"};
static const char *program_attribute[8] = {"Type", "Offset", "VirtAddr", "PhysAddr", "FileSiz", "MemSiz", "Flags", "Align"};
static const char *dynamic_symbol_attribute[8] = {"Num", "Value", "Size", "Type", "Bind", "Vis", "Ndx", "Name"};
static const char *relocatable_attribute[5] = {"Offset", "Info", "Type", "Sym. Value", "Sym .Name + Addend"};
static const char *flag_section_keyword_infos = "\nKey to FLAGS:\nW [Write] - A [Alloc] - X [Execute] - M [Merge]\nS [Strings] - I [Infos] - L [Link order]\nO [Extra OS processing required] - G [Group]\nT [TLS] - E [Exclude] - C [Compressed]";
static const char *flag_program_keyword_infos = "\nKey to FLAGS:\nR [READ] - W [Write] - X [Execute]";
static const char *no_program_headers = "There are no program headers in this file.";
static const char *no_section_headers = "There are no section headers in this file.";
static const char *no_symbol_section = "There is no symbol section in this file.";
static const char *no_dynamic_section = "There is no dynamic section in this file.";

// Enum declaration
typedef enum {
    STATIC,
    DYNAMIC
} SYMBOL;

typedef enum {
    ALL,
    HEADER,
    SECTION_HEADER,
    PROGRAM_HEADER,
    STATIC_SYMBOL,
    DYNAMIC_SYMBOL,
    RELOCATABLE
} OPTIONS;

OPTIONS options = 0;

// Structures declaration
typedef struct {
    ElfW(Shdr) * dynamic_symbol;
    ElfW(Shdr) * symbol;
    size_t str_dynamic_symbol_off;
    size_t str_symbol_off;
} section_info;

typedef struct {
    size_t value;
    const char *string;
} xlat;

// Xlat arrays

// Machines types
xlat e_machine[] =
        {
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
                XLAT_END};

// Sections types
xlat sh_type[] =
        {
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
                XLAT(SHT_PREINIT_ARRAY),
                XLAT(SHT_INIT_ARRAY),
                XLAT(SHT_FINI_ARRAY),
                XLAT_END};

xlat p_type[] =
        {
                XLAT(PT_NULL),
                XLAT(PT_LOAD),
                XLAT(PT_DYNAMIC),
                XLAT(PT_INTERP),
                XLAT(PT_NOTE),
                XLAT(PT_SHLIB),
                XLAT(PT_PHDR),
                XLAT(PT_LOSUNW),
                XLAT(PT_SUNWBSS),
                XLAT(PT_SUNWSTACK),
                XLAT(PT_HISUNW),
                XLAT(PT_LOPROC),
                XLAT(PT_HIPROC),
                XLAT(PT_GNU_RELRO),
                XLAT(PT_GNU_STACK),
                XLAT(PT_GNU_EH_FRAME),
                XLAT_END};

xlat dyn_sym_type[] =
        {
                XLAT(STT_NOTYPE),
                XLAT(STT_OBJECT),
                XLAT(STT_FUNC),
                XLAT(STT_SECTION),
                XLAT(STT_FILE),
                XLAT(STT_COMMON),
                XLAT(STT_TLS),
                XLAT(STT_LOOS),
                XLAT(STT_HIOS),
                XLAT(STT_LOPROC),
                XLAT(STT_SPARC_REGISTER),
                XLAT(STT_HIPROC),
                XLAT_END};

xlat dyn_sym_bind[] =
        {
                XLAT(STB_LOCAL),
                XLAT(STB_GLOBAL),
                XLAT(STB_WEAK),
                XLAT(STB_LOOS),
                XLAT(STB_HIOS),
                XLAT(STB_LOPROC),
                XLAT(STB_HIPROC),
                XLAT_END};

xlat dyn_sym_vis[] =
        {
                XLAT(STV_DEFAULT),
                XLAT(STV_INTERNAL),
                XLAT(STV_HIDDEN),
                XLAT(STV_PROTECTED),
                XLAT_END};

xlat dyn_sym_index[] =
        {
                XLAT(SHN_UNDEF),
                XLAT(SHN_LORESERVE),
                XLAT(SHN_LOPROC),
                XLAT(SHN_ABS),
                XLAT(SHN_COMMON),
                XLAT(SHN_HIRESERVE),
                XLAT_END};

#endif//READELF_H
