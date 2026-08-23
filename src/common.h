#ifndef COMMON_H
#define COMMON_H


#include <stdalign.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>


/// General


typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ullong;

typedef long long llong;

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;
typedef uint64_t qword;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef size_t usize;

typedef float f32;
typedef double f64;


#define UNUSED(x) (void)(x)


#define LENOF(xs) (sizeof (xs) / sizeof *(xs))


enum
{
  KB = 1024,
  MB = 1024 * KB,
  GB = 1024 * MB,
};


/// Alignment


enum
{
  ALIGN_MAX = alignof (max_align_t),
};


usize
align_forward_usize (usize x, usize a);

void *
align_forward_p (void *p, usize a);


/// Status


#define SERROR(S) ((S != NULL) && *(int *)(S) != 0)


#define sset(S, x) sset_explicit ((S), (x), __FILE__, __LINE__, __func__)


void
sset_explicit (int *S, int x, const char *file, int line, const char *function);


/// Allocation


#define memset0(x) memset ((x), 0, sizeof *(x))


void *
allocate (int *S, usize size);

void *
allocate0 (int *S, usize size);

void *
reallocate (int *S, void *p, usize size);


void *
allocate_n (int *S, usize n, usize size);

void *
allocate0_n (int *S, usize n, usize size);

void *
reallocate_n (int *S, void *p, usize n, usize size);


void
deallocate (void *p);


/// Debug


#define HERE __FILE__, __LINE__, __func__


noreturn void
panic (const char *fmt, ...);


noreturn void
panic_at (const char *file, int line, const char *function, const char *fmt, ...);


#define panic_assert(condition)                                                                    \
  if (condition)                                                                                   \
    ;                                                                                              \
  else                                                                                             \
    panic_at (HERE, "assertion `%s` failed", #condition)


#define panic_asserts(condition, s)                                                                \
  if (condition)                                                                                   \
    ;                                                                                              \
  else                                                                                             \
    panic_at (HERE, "%s", s)


#define panic_unreachable()                                                                        \
  panic_at (HERE, "unreachable code reached")


/// Fixed Array


#define fa_reserve(S, fa, expected)                                                                \
  do                                                                                               \
    {                                                                                              \
      (fa)->data = allocate_n ((S), (expected), sizeof *(fa)->data);                               \
    }                                                                                              \
  while (0)


#define fa_append(fa, item)                                                                        \
  do                                                                                               \
    {                                                                                              \
      (fa)->data[(fa)->len++] = item;                                                              \
    }                                                                                              \
  while (0)


#define fa_append_uninitialized(fa)                                                                \
  do                                                                                               \
    {                                                                                              \
      (fa)->len++;                                                                                 \
    }                                                                                              \
  while (0)


#define fa_top(fa)                                                                                 \
  (fa)->data[(fa)->len - 1]


#define fa_top2(fa)                                                                                \
  (fa)->data[(fa)->len - 2]


#define fa_pop(fa)                                                                                 \
  do                                                                                               \
    {                                                                                              \
      (fa)->len -= 1;                                                                              \
    }                                                                                              \
  while (0)


#define fa_clear(fa)                                                                               \
  do                                                                                               \
    {                                                                                              \
      deallocate ((fa)->data);                                                                     \
                                                                                                   \
      (fa)->data = NULL;                                                                           \
      (fa)->len = 0;                                                                               \
    }                                                                                              \
  while (0)


/// Dynamic Array


#define da_reserve(S, da, expected)                                                                \
  do                                                                                               \
    {                                                                                              \
      if ((expected) > (da)->cap)                                                                  \
        {                                                                                          \
          if ((da)->cap == 0)                                                                      \
            (da)->cap = 8;                                                                         \
                                                                                                   \
          while ((expected) > (da)->cap)                                                           \
            (da)->cap *= 2;                                                                        \
                                                                                                   \
          (da)->data = reallocate_n ((S), (da)->data, (da)->cap, sizeof *(da)->data);              \
        }                                                                                          \
    }                                                                                              \
  while (0)


#define da_append(S, da, item)                                                                     \
  do                                                                                               \
    {                                                                                              \
      da_reserve ((S), (da), (da)->len + 1);                                                       \
                                                                                                   \
      if (!SERROR ((S)))                                                                           \
        (da)->data[(da)->len++] = item;                                                            \
    }                                                                                              \
  while (0)


#define da_append_uninitialized(S, da)                                                             \
  do                                                                                               \
    {                                                                                              \
      da_reserve ((S), (da), (da)->len + 1);                                                       \
                                                                                                   \
      if (!SERROR ((S)))                                                                           \
        (da)->len++;                                                                               \
    }                                                                                              \
  while (0)


#define da_extend(S, da, buffer, n)                                                                \
  do                                                                                               \
    {                                                                                              \
      da_reserve ((S), (da), (da)->len + (n));                                                     \
                                                                                                   \
      if (!SERROR ((S)))                                                                           \
        {                                                                                          \
          memcpy ((da)->data + (da)->len, (buffer), (n) * sizeof *(da)->data);                     \
                                                                                                   \
          (da)->len += (n);                                                                        \
        }                                                                                          \
    }                                                                                              \
  while (0)


#define da_top(da)                                                                                 \
  (da)->data[(da)->len - 1]


#define da_top2(da)                                                                                \
  (da)->data[(da)->len - 2]


#define da_pop(da)                                                                                 \
  do                                                                                               \
    {                                                                                              \
      (da)->len -= 1;                                                                              \
    }                                                                                              \
  while (0)


#define da_clear(da)                                                                               \
  do                                                                                               \
    {                                                                                              \
      deallocate ((da)->data);                                                                     \
                                                                                                   \
      (da)->data = NULL;                                                                           \
      (da)->len = 0;                                                                               \
      (da)->cap = 0;                                                                               \
    }                                                                                              \
  while (0)


/// Hash Table


#define ht_init(S, ht, n)                                                                          \
  do                                                                                               \
    {                                                                                              \
      (ht)->len = (n);                                                                             \
                                                                                                   \
      fa_reserve ((S), (ht), (ht)->len);                                                           \
                                                                                                   \
      if (!SERROR (S))                                                                             \
        memset ((ht)->data, 0, sizeof *((ht)->data) * (ht)->len);                                  \
    }                                                                                              \
  while (0)


#define ht_clear(ht)                                                                               \
  do                                                                                               \
    {                                                                                              \
      for (usize i = 0; i < (ht)->len; i++)                                                        \
        da_clear (&(ht)->data[i]);                                                                 \
                                                                                                   \
      fa_clear (ht);                                                                               \
    }                                                                                              \
  while (0)


#define HT_GENERATE_APPEND(S, ht, item_key, item_value, hash, compare)                             \
  const usize k = (hash) ((item_key)) % (ht)->len;                                                 \
                                                                                                   \
  for (usize i = 0; i < (ht)->data[k].len; i++)                                                    \
    if ((compare) ((ht)->data[k].data[i].key, (item_key)))                                         \
      {                                                                                            \
        (ht)->data[k].data[i].value = (item_value);                                                \
        return;                                                                                    \
      }                                                                                            \
                                                                                                   \
  const usize i = (ht)->data[k].len;                                                               \
                                                                                                   \
  da_append_uninitialized ((S), &(ht)->data[k]);                                                   \
                                                                                                   \
  if (!SERROR (S))                                                                                 \
    {                                                                                              \
      (ht)->data[k].data[i].key   = (item_key);                                                    \
      (ht)->data[k].data[i].value = (item_value);                                                  \
    }                                                                                              \


#define HT_GENERATE_FIND(ht, item_key, result_value, hash, compare)                                \
  const usize k = (hash) ((item_key)) % (ht)->len;                                                 \
                                                                                                   \
  for (usize i = 0; i < (ht)->data[k].len; i++)                                                    \
    if ((compare) ((ht)->data[k].data[i].key, (item_key)))                                         \
      {                                                                                            \
        if ((result_value))                                                                        \
          *(result_value) = (ht)->data[k].data[i].value;                                           \
        return true;                                                                               \
      }                                                                                            \
                                                                                                   \
  return false;                                                                                    \


/// Math


usize
usize_digits (usize n);


#define GENERATE_MIN_MAX(T)                                                                        \
  static inline T                                                                                  \
  min_##T (T a, T b)                                                                               \
  {                                                                                                \
    if (a < b)                                                                                     \
      return a;                                                                                    \
    return b;                                                                                      \
  }                                                                                                \
                                                                                                   \
  static inline T                                                                                  \
  max_##T (T a, T b)                                                                               \
  {                                                                                                \
    if (a > b)                                                                                     \
      return a;                                                                                    \
    return b;                                                                                      \
  }


GENERATE_MIN_MAX (uchar)
GENERATE_MIN_MAX (ushort)
GENERATE_MIN_MAX (uint)
GENERATE_MIN_MAX (ulong)
GENERATE_MIN_MAX (ullong)

GENERATE_MIN_MAX (char)
GENERATE_MIN_MAX (short)
GENERATE_MIN_MAX (int)
GENERATE_MIN_MAX (long)
GENERATE_MIN_MAX (llong)

GENERATE_MIN_MAX (u8)
GENERATE_MIN_MAX (u16)
GENERATE_MIN_MAX (u32)
GENERATE_MIN_MAX (u64)

GENERATE_MIN_MAX (s8)
GENERATE_MIN_MAX (s16)
GENERATE_MIN_MAX (s32)
GENERATE_MIN_MAX (s64)

GENERATE_MIN_MAX (usize)

GENERATE_MIN_MAX (f32)
GENERATE_MIN_MAX (f64)


#undef GENERATE_MIN_MAX


#define GENERATE_CLAMP(T)                                                                          \
  static inline T                                                                                  \
  clamp_##T (T x, T lo, T hi)                                                                      \
  {                                                                                                \
    if (x < lo)                                                                                    \
      return lo;                                                                                   \
    if (x > hi)                                                                                    \
      return hi;                                                                                   \
    return x;                                                                                      \
  }


GENERATE_CLAMP (uchar)
GENERATE_CLAMP (ushort)
GENERATE_CLAMP (uint)
GENERATE_CLAMP (ulong)
GENERATE_CLAMP (ullong)

GENERATE_CLAMP (char)
GENERATE_CLAMP (short)
GENERATE_CLAMP (int)
GENERATE_CLAMP (long)
GENERATE_CLAMP (llong)

GENERATE_CLAMP (u8)
GENERATE_CLAMP (u16)
GENERATE_CLAMP (u32)
GENERATE_CLAMP (u64)

GENERATE_CLAMP (s8)
GENERATE_CLAMP (s16)
GENERATE_CLAMP (s32)
GENERATE_CLAMP (s64)

GENERATE_CLAMP (usize)

GENERATE_CLAMP (f32)
GENERATE_CLAMP (f64)


#undef GENERATE_CLAMP


/// C String


char *
sprintf_static (char *format, ...);


/// String


struct str;
struct sv;
struct sb;


#define STR_PRI "%.*s"

#define str_pri(s) (s32)((s).len), (s).data


#define STR0 (struct str){ 0 }


struct str
{
  char *data;
  usize len;
};


struct str
str_from_cstrn (int *S, const char *s, usize n);

struct str
str_from_cstr (int *S, const char *s);

struct str
str_from_str (int *S, struct str str);

struct str
str_from_sv (int *S, struct sv sv);


#define str_from(S, x)                                                                          \
  _Generic (                                                                                       \
    (x),                                                                                           \
    char *: str_from_cstr,                                                                         \
    const char *: str_from_cstr,                                                                   \
    struct str: str_from_str,                                                                      \
    struct sv: str_from_sv                                                                         \
  ) ((S), (x))                                                                                \


void
str_to_cstrn (struct str str, char *buffer, usize n);


void
str_clear (struct str *str);


/// String View


#define SV_PRI "%.*s"

#define sv_pri(s) (s32)((s).len), (s).data


#define SV0 (struct sv){ 0 }


struct sv
{
  const char *data;
  usize len;
};


struct sv
sv_from_cstrn (const char *s, usize n);

struct sv
sv_from_cstr (const char *s);

struct sv
sv_from_str (struct str str);

struct sv
sv_from_sv (struct sv sv);


#define sv_from(x)                                                                                 \
  _Generic (                                                                                       \
    (x),                                                                                           \
    char *: sv_from_cstr,                                                                          \
    const char *: sv_from_cstr,                                                                    \
    struct str: sv_from_str,                                                                       \
    struct sv: sv_from_sv                                                                          \
  ) ((x))


void
sv_to_cstrn (struct sv sv, char *buffer, usize n);


const char *
sv_begin (struct sv sv);

const char *
sv_end (struct sv sv);


char
sv_first (struct sv sv);

char
sv_last (struct sv sv);


void
sv_popl (struct sv *sv, usize n);

void
sv_popr (struct sv *sv, usize n);


void
sv_stripl (struct sv *sv);

void
sv_stripr (struct sv *sv);

void
sv_strip (struct sv *sv);


// Searching & Testing


#define SV_NPOS ((usize)-1)


usize
sv_find_cstrn (struct sv haystack, const char *s, usize n);

usize
sv_find_cstr (struct sv haystack, const char *s);

usize
sv_find_str (struct sv haystack, struct str str);

usize
sv_find_sv (struct sv haystack, struct sv sv);


#define sv_find(haystack, x)                                                                       \
  _Generic (                                                                                       \
    (x),                                                                                           \
    char *: sv_find_cstr,                                                                          \
    const char *: sv_find_cstr,                                                                    \
    struct str: sv_find_str,                                                                       \
    struct sv: sv_find_sv                                                                          \
  ) ((haystack), (x))


bool
sv_contains_cstrn (struct sv haystack, const char *s, usize n);

bool
sv_contains_cstr (struct sv haystack, const char *s);

bool
sv_contains_str (struct sv haystack, struct str str);

bool
sv_contains_sv (struct sv haystack, struct sv sv);


#define sv_contains(haystack, x)                                                                   \
  _Generic (                                                                                       \
    (x),                                                                                           \
    char *: sv_contains_cstr,                                                                      \
    const char *: sv_contains_cstr,                                                                \
    struct str: sv_contains_str,                                                                   \
    struct sv: sv_contains_sv                                                                      \
  ) ((haystack), (x))


// Slicing


struct sv
sv_slice (struct sv sv, usize start, usize end);


/// String Builder


#define SB_PRI "%.*s"

#define sb_pri(s) (s32)((s).len), (s).data


#define SB0 (struct sb){ 0 }


struct sb
{
  char *data;
  usize len;
  usize cap;
};


struct sb
sb_from_cstrn (int *S, const char *s, usize n);

struct sb
sb_from_cstr (int *S, const char *s);

struct sb
sb_from_str (int *S, struct str str);

struct sb
sb_from_sv (int *S, struct sv sv);


#define sb_from(x)                                                                                 \
  _Generic (                                                                                       \
    (x),                                                                                           \
    char *: sb_from_cstr,                                                                          \
    const char *: sb_from_cstr,                                                                    \
    struct str: sb_from_str,                                                                       \
    struct sv: sb_from_sv                                                                          \
  ) ((x))


void
sb_to_cstrn (const struct sb *sb, char *buffer, usize n);


struct str
sb_into_str (struct sb *sb);


#define sb_terminate(S, sb) sb_append ((S), (sb), 0)


void
sb_append (int *S, struct sb *sb, char c);


void
sb_extend_cstrn (int *S, struct sb *sb, const char *s, usize n);

void
sb_extend_cstr (int *S, struct sb *sb, const char *s);

void
sb_extend_str (int *S, struct sb *sb, struct str str);

void
sb_extend_sv (int *S, struct sb *sb, struct sv sv);


#define sb_extend(sb, x)                                                                           \
  _Generic (                                                                                       \
    (x),                                                                                           \
    char *: sb_extend_cstr,                                                                        \
    const char *: sb_extend_cstr,                                                                  \
    struct str: sb_extend_str,                                                                     \
    struct sv: sb_extend_sv                                                                        \
  ) ((sb), (x))


void
sb_extend_fmt (int *S, struct sb *sb, const char *fmt, ...);


void
sb_read_file (int *S, struct sb *sb, const char *path);


void
sb_clear (struct sb *sb);


/// Custom


#define indent_dump(n) printf ("%*s", (int)(n), "")


f32
random_f32 (f32 lo, f32 hi);


#endif // COMMON_H

