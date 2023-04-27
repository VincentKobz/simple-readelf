#include "tools.h"
#include <string.h>
#include <err.h>

void auto_pad_number(int number, size_t target_length)
{
    char number_str[16] = {0};
    if (sprintf(number_str, "%d", number) < 0)
    {
        err(1, "Error during convert number to string !");
    }
    auto_pad(number_str, target_length);
}

// Write on stdout the string and pad it with spaces to respect the target total length
void auto_pad(const char *string, size_t target_length)
{
    size_t i = 0;
    for (i = 0; i < strlen(string) && i < target_length - 1; i++)
    {
        putchar(string[i]);
    }

    for (; i < target_length; i++)
    {
        putchar(' ');
    }
}
