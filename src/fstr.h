#ifndef _SCOTTZ0R_FSTR_H_INCLUDE_GUARD
#define _SCOTTZ0R_FSTR_H_INCLUDE_GUARD

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum fstr_error_t
{
    FSTR_OK,
    FSTR_MISUSE,
    FSTR_OVERFLOW
} fstr_error_t;

typedef unsigned char fstr_size_t;

typedef struct fstr
{
    char* str;
    fstr_size_t len;
    fstr_size_t capacity;
} fstr;

fstr_error_t fstr_init(fstr* str, char* buffer, fstr_size_t size);

fstr_error_t fstr_itofstr(int value, fstr* dst, int radix);

fstr_error_t fstr_reverse(fstr* dst);

fstr_error_t fstr_rpad(fstr* dst);

fstr_error_t fstr_strcat(fstr* dst, const char* src);

fstr_error_t fstr_strcpy(fstr* dst, const char* src);

fstr_size_t fstr_strlen(fstr* str);

#ifdef __cplusplus
}
#endif

#endif // _SCOTTZ0R_FSTR_H_INCLUDE_GUARD
