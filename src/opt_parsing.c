#include "opt_parsing.h"
#include "stdio.h"
#include <err.h>
#include <getopt.h>

char *parse_options(int argc, char **argv, OPTIONS *options) {
    char *filename = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "a:h:P:S:s:d:")) != -1) {
        switch (opt) {
            case 'a':
                *options = ALL;
                filename = optarg;
                break;
            case 'h':
                *options = HEADER;
                filename = optarg;
                break;
            case 'P':
                *options = PROGRAM_HEADER;
                filename = optarg;
                break;
            case 'S':
                *options = SECTION_HEADER;
                filename = optarg;
                break;
            case 's':
                *options = STATIC_SYMBOL;
                filename = optarg;
                break;
            case 'd':
                *options = DYNAMIC_SYMBOL;
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
