#ifndef READELF_H
#define READELF_H

#define XLAT(val) {(size_t)(val), #val}
#define XLAT_END {0, 0}

struct xlat {
    size_t value;
    const char *string;
};

// Different types of machines
struct xlat e_machine[] = {
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

#endif
