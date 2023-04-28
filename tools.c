#include "tools.h"
#include <string.h>
#include <err.h>

void auto_pad_number(int number, const char *format, size_t target_length, int is_address)
{
    char number_str[16] = {0};
    // Convert input string to right number format
    if (sprintf(number_str, format, number) < 0)
    {
        err(1, "Error during convert number to string !");
    }

    if (is_address)
    {
        char temp[16] = {0};
        size_t number_str_len = strlen(number_str);

        // Copy number_str to temp
        for (size_t i = 0; i < number_str_len; i++)
        {
            temp[i] = number_str[i];
        }
        // Pad with zeros
        for (size_t i = 0; i < 15; i++)
        {
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
