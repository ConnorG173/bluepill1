
#include "uprintf.h"

int push_char(usart_t* u, char ch)
{
    return (int)usart_tx_push(u, (ui16)ch);
}

int push_string(usart_t* u, char* s) //returns total # of chars pushed
{
    int totalpushed = 0;
    if (s == NULL) { return (int)push_string(u, "(null)"); }
    else
    {
        while (*s != '\0')
        {
            totalpushed += (int)push_char(u, *s);
            s += sizeof(char);
        }
    }
    return totalpushed;
}

int push_int32(usart_t* u, long x)
{
    int totalpushed = 0;
    if (x < 0)
    {
        totalpushed += (int)push_char(u, '-'); //append minus sign before numbers
        x *= -1; //change to positive for formatting
    }
    char numstr[10] = {'0','0','0','0','0','0','0','0','0','0'};
    char digits[10] = {'0','1','2','3','4','5','6','7','8','9'};
    int idx = 9;
    if (x == 0) { return push_char(u, '0'); }
    while (x > 0)
    {
        numstr[idx] = digits[x % 10];
        x /= 10; idx--;
    }
    for (int i = idx + 1; i < 10; i++)
    {
        totalpushed += (int)push_char(u, numstr[i]);
    }
    return totalpushed;
}

int push_uint32(usart_t* u, ui32 x)
{
    int totalpushed = 0;
    char numstr[10] = {'0','0','0','0','0','0','0','0','0','0'};
    char digits[10] = {'0','1','2','3','4','5','6','7','8','9'};
    int idx = 9;
    if (x == 0) { return push_char(u, '0'); }
    while (x > 0)
    {
        numstr[idx] = digits[x % 10];
        x /= 10; idx--;
    }
    for (int i = idx + 1; i < 10; i++)
    {
        totalpushed += push_char(u, numstr[i]);
    }
    return totalpushed;
}

int push_hex(usart_t* u, ui32 num, bool is_upper)
{
    int totalpushed = 0;
    char hexstr[10] = {'0','x','0','0','0','0','0','0','0','0'};
    char hexdigitsupper[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    char hexdigitslower[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    int idx = 9;
    if (num == 0) { return push_string(u, "0x0"); }
    while (num > 0)
    {
        if (is_upper) { hexstr[idx] = hexdigitsupper[num % 16]; }
        else { hexstr[idx] = hexdigitslower[num % 16]; }
        num /= 16; idx--;
    }
    for (int i = idx + 1; i < 10; i++)
    {
        totalpushed += (int)push_char(u, hexstr[i]);
    }
    return totalpushed;
}

int handle_format_spec(char specifier, usart_t* u, va_list args)
{
    switch (specifier)
    {
        case ('c'):
        {
            return push_char(u, va_arg(args, int));
        }
        case ('s'):
        {
            return push_string(u, va_arg(args, char*));
        }
        case ('d'):
        {
            return push_int32(u, va_arg(args, int));
        }
        case ('u'):
        {
            return push_uint32(u, (ui32)va_arg(args, unsigned int));
        }
        case ('x'):
        {
            return push_hex(u, (ui32)va_arg(args, unsigned int), false);
        }
        case ('X'):
        {
            return push_hex(u, (ui32)va_arg(args, unsigned int), true);
        }
        case ('%'):
        {
            return push_char(u, '%');
        }
        default:
        {
            int x = 0;
            x += (int)push_char(u, '%');
            return x + (int)push_char(u, specifier);
        }
    }
}

int uvprintf(usart_t* u, const char* fmt, va_list args)
{
    typedef enum state 
    {
        TEXT,
        FORMAT
    } STATE;
    STATE state = TEXT;

    int totalpushed = 0;
    char* idx = fmt;
    while (*idx != '\0')
    {
        char ch = *idx;
        if (state == TEXT)
        {
            if (ch == '%') { state = FORMAT; }
            else { totalpushed += push_char(u, ch); }
        }
        else if (state == FORMAT)
        {
            totalpushed += handle_format_spec(ch, u, args);
            state = TEXT;
        }
        idx += sizeof(char);
    }
    return totalpushed;
}

int uprintf(usart_t* u, const char* format, ...) //Returns total chars pushed to tx buffer
{
    va_list args;
    va_start(args, format);
    int n = uvprintf(u, format, args);
    va_end(args);
    return n;
}







