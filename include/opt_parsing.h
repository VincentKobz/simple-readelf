#ifndef OPT_PARSING_H
#define OPT_PARSING_H

typedef enum {
    ALL,
    HEADER,
    SECTION_HEADER,
    PROGRAM_HEADER,
    STATIC_SYMBOL,
    DYNAMIC_SYMBOL
} OPTIONS;

char *parse_options(int argc, char **argv, OPTIONS *options);

#endif//OPT_PARSING_H
