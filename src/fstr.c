#include "fstr.h"

static fstr_error_t itofsr_r10(int value, fstr* dst);

static fstr_error_t itofsr_unsigned(unsigned value, fstr* dst, int radix);

fstr_error_t fstr_init(fstr* str, char* buffer, fstr_size_t size)
{
    // Guards:
    if (!str || !buffer || size == 0)
    {
        return FSTR_MISUSE;
    }

    str->str = buffer;
    str->len = 0;
    str->capacity = size;
    *(str->str) = 0;

    return FSTR_OK;
}

fstr_error_t fstr_strcat(fstr* dst, const char* src)
{
    // Guards:
    if (!dst || !src)
    {
        return FSTR_MISUSE;
    }

    fstr_error_t rc = FSTR_OK;
    char* p = dst->str + dst->len;
    char* end = dst->str + dst->capacity - 1;
    const char* s = src;

    while (p < end && *s != 0)
    {
        *p = *s;
        ++p;
        ++s;
    }

    // If source did not end on a null char, then there is not enough space in dst.
    if (*s != 0)
    {
        rc = FSTR_OVERFLOW;
    }

    *p = 0;

    dst->len = p - dst->str;

    return rc;
}

fstr_error_t fstr_strcpy(fstr* dst, const char* src)
{
    // Guards:
    if (!dst || !src)
    {
        return FSTR_MISUSE;
    }

    fstr_error_t rc = FSTR_OK;
    char* p = dst->str;
    char* end = dst->str + dst->capacity - 1;
    const char* s = src;

    while (p < end && *s != 0)
    {
        *p = *s;
        ++p;
        ++s;
    }

    // If source did not end on a null char, then there is not enough space in dst.
    if (*s != 0)
    {
        rc = FSTR_OVERFLOW;
    }

    *p = 0;

    dst->len = p - dst->str;

    return rc;
}

fstr_size_t fstr_strlen(fstr* str)
{
    // Guards:
    if (!str)
    {
        return 0;
    }

    return str->len;
}

fstr_error_t fstr_itofstr(int value, fstr* dst, int radix)
{
    if (radix == 10)
    {
        return itofsr_r10(value, dst);
    }
    else
    {
        return itofsr_unsigned((unsigned)value, dst, radix);
    }
}

fstr_error_t fstr_reverse(fstr* dst)
{
    // Guards:
    if (!dst)
    {
        return FSTR_MISUSE;
    }

    char* f;
    char* e;

    for (f = dst->str, e = dst->str + dst->len - 1; f < e; ++f, --e)
    {
        char tmp = *f;
        *f = *e;
        *e = tmp;
    }

    return FSTR_OK;;
}

fstr_error_t fstr_rpad(fstr* dst)
{
    if (!dst)
    {
        return FSTR_MISUSE;
    }

    char* p = dst->str + dst->len;
    char* e = dst->str + dst->capacity - 1;

    for (; p < e; ++p)
    {
        *p = ' ';
    }

    *e = 0;

    return FSTR_OK;
}

static fstr_error_t itofsr_r10(int value, fstr* dst)
{
    // Guards:
    if (!dst)
    {
        return FSTR_MISUSE;
    }

    // Make end pointer one less to ensure there will be space for a null terminator.
    char* p_end = dst->str + dst->capacity - 1;
    char* p_buff = dst->str;
    char neg = 0;
    fstr_error_t rc = FSTR_OK;

    if (value < 0)
    {
        neg = 1;
        value = -value;
    }

    int v = value;

    do
    {
        int c = v % 10;
        *p_buff = '0' + c;
        ++p_buff;
        v /= 10;
    } while (v && p_buff < p_end);

    // Failure condition: out of space in buffer.
    if (v > 0)
    {
        rc = FSTR_OVERFLOW;
    }

    if (neg)
    {
        if (p_buff < p_end)
        {
            *p_buff = '-';
            ++p_buff;
        }
        else
        {
            // Failure condition. Not enough space for negative sign.
            rc = FSTR_OVERFLOW;
        }
    }

    // Always null terminate, regardless of failure conditions.
    *p_buff = 0;
    dst->len = p_buff - dst->str;

    if (rc == FSTR_OK)
    {
        fstr_reverse(dst);
    }

    return rc;
}

static fstr_error_t itofsr_unsigned(unsigned value, fstr* dst, int radix)
{
    // Guards:
    if (!dst)
    {
        return FSTR_MISUSE;
    }

    // Make end pointer one less to ensure there will be space for a null terminator.
    char* p_end = dst->str + dst->capacity - 1;
    char* p_buff = dst->str;
    fstr_error_t rc = FSTR_OK;

    unsigned v = value;

    do
    {
        unsigned c = v % radix;

        if (c <= 9)
        {
            *p_buff = '0' + c;
        }
        else
        {
            *p_buff = 'a' + (c - 10);
        }

        ++p_buff;
        v /= radix;
    } while (v && p_buff < p_end);

    // Failure condition: out of space in buffer.
    if (v > 0)
    {
        rc = FSTR_OVERFLOW;
    }

    // Always null terminate, regardless of failure conditions.
    *p_buff = 0;
    dst->len = p_buff - dst->str;

    if (rc == FSTR_OK)
    {
        fstr_reverse(dst);
    }

    return rc;
}
