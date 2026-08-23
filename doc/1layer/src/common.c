#include "common.h"
#include <ctype.h>
#include <stdarg.h>
#include <string.h>


/// General


/// Alignment


usize
align_forward_usize (usize x, usize a)
{
  usize m = a - 1;

  return (x + m) & ~m;
}


void *
align_forward_p (void *p, usize a)
{
  uintptr_t x = (uintptr_t)p;
  uintptr_t m = a - 1;

  return (void *)((x + m) & ~m);
}


/// Status


void
sset_explicit (int *S, int x, const char *file, int line, const char *function)
{
  if (S)
    *S = x;
  else
    if (SERROR (&x))
      panic_at (file, line, function, "sset %d", x);
}


/// Allocation


void *
allocate (int *S, usize size)
{
  void *p = malloc (size);

  if (!p)
    {
      sset (S, -1);

      return NULL;
    }

  return p;
}


void *
allocate0 (int *S, usize size)
{
  void *p = malloc (size);

  if (!p)
    {
      sset (S, -1);

      return NULL;
    }

  memset (p, 0, size);

  return p;
}


void *
reallocate (int *S, void *p, usize size)
{
  void *q = realloc (p, size);

  if (!q)
    {
      sset (S, -1);

      return p;
    }

  return q;
}


void *
allocate_n (int *S, usize n, usize size)
{
  return allocate (S, n * size);
}


void *
allocate0_n (int *S, usize n, usize size)
{
  return allocate0 (S, n * size);
}


void *
reallocate_n (int *S, void *p, usize n, usize size)
{
  return reallocate (S, p, n * size);
}


void
deallocate (void *p)
{
  if (!p)
    return;

  free (p);
}


/// Debug


noreturn void
panic (const char *fmt, ...)
{
  va_list va;

  va_start (va, fmt);

  fprintf (stderr, "panic: ");

  vfprintf (stderr, fmt, va);

  fprintf (stderr, "\n");

  fflush (stderr);

  va_end (va);

  abort ();
}


noreturn void
panic_at (const char *file, int line, const char *function, const char *fmt, ...)
{
  va_list va;

  va_start (va, fmt);

  fprintf (stderr, "panic: %s:%d: %s(): ", file, line, function);

  vfprintf (stderr, fmt, va);

  fprintf (stderr, "\n");

  fflush (stderr);

  va_end (va);

  abort ();
}


/// Fixed Array


/// Dynamic Array


/// Hash Table


/// Math


usize
usize_digits (usize n)
{
  usize d = 0;

  while (n)
    {
      d += 1;
      n /= 10;
    }

  return d;
}


/// C String


char *
sprintf_static (char *fmt, ...)
{
  static char buffer[4096];

  va_list args;

  va_start (args, fmt);

  vsnprintf (buffer, sizeof buffer, fmt, args);

  va_end (args);

  return buffer;
}


/// String


struct str
str_from_cstrn (int *S, const char *s, usize n)
{
  struct str str;

  str.data = allocate (S, n);

  if (SERROR (S))
    return STR0;

  str.len = n;

  memcpy (str.data, s, n);

  return str;
}


struct str
str_from_cstr (int *S, const char *s)
{
  return str_from_cstrn (S, s, strlen (s));
}


struct str
str_from_str (int *S, struct str str)
{
  return str_from_cstrn (S, str.data, str.len);
}


struct str
str_from_sv (int *S, struct sv sv)
{
  return str_from_cstrn (S, sv.data, sv.len);
}


void
str_to_cstrn (struct str str, char *buffer, usize n)
{
  usize size = str.len;

  if (size > n - 1)
    size = n - 1;

  memcpy (buffer, str.data, size);

  buffer[size] = '\0';
}


void
str_clear (struct str *str)
{
  deallocate (str->data);

  str->data = NULL;
  str->len = 0;
}


/// String View


struct sv
sv_from_cstrn (const char *s, usize n)
{
  struct sv sv;

  sv.data = s;
  sv.len = n;

  return sv;
}


struct sv
sv_from_cstr (const char *s)
{
  return sv_from_cstrn (s, strlen (s));
}


struct sv
sv_from_str (struct str str)
{
  return sv_from_cstrn (str.data, str.len);
}


struct sv
sv_from_sv (struct sv sv)
{
  return sv_from_cstrn (sv.data, sv.len);
}


void
sv_to_cstrn (struct sv sv, char *buffer, usize n)
{
  usize size = sv.len;

  if (size > n - 1)
    size = n - 1;

  memcpy (buffer, sv.data, size);

  buffer[size] = '\0';
}


const char *
sv_begin (struct sv sv)
{
  return sv.data;
}


const char *
sv_end (struct sv sv)
{
  return sv.data + sv.len;
}


char
sv_first (struct sv sv)
{
  return sv.data[0];
}


char
sv_last (struct sv sv)
{
  return sv.data[sv.len - 1];
}


void
sv_popl (struct sv *sv, usize n)
{
  n = min_usize (n, sv->len);

  sv->data += n;
  sv->len -= n;
}


void
sv_popr (struct sv *sv, usize n)
{
  n = min_usize (n, sv->len);

  sv->len -= n;
}


void
sv_stripl (struct sv *sv)
{
  while (sv->len > 0 && isspace ((uchar)sv_first (*sv)))
    sv_popl (sv, 1);
}


void
sv_stripr (struct sv *sv)
{
  while (sv->len > 0 && isspace ((uchar)sv_last (*sv)))
    sv_popr (sv, 1);
}


void
sv_strip (struct sv *sv)
{
  sv_stripl (sv);
  sv_stripr (sv);
}


// Searching & Testing


usize
sv_find_cstrn (struct sv haystack, const char *s, usize n)
{
  if (n == 0)
    return 0;

  if (n > haystack.len)
    return SV_NPOS;

  for (usize i = 0; i <= haystack.len - n; i++)
    {
      if (memcmp (haystack.data + i, s, n) == 0)
        return i;
    }

  return SV_NPOS;
}


usize
sv_find_cstr (struct sv haystack, const char *s)
{
  return sv_find_cstrn (haystack, s, strlen (s));
}


usize
sv_find_str (struct sv haystack, struct str str)
{
  return sv_find_cstrn (haystack, str.data, str.len);
}


usize
sv_find_sv (struct sv haystack, struct sv sv)
{
  return sv_find_cstrn (haystack, sv.data, sv.len);
}


bool
sv_contains_cstrn (struct sv haystack, const char *s, usize n)
{
  if (n == 0)
    return true;

  if (n > haystack.len)
    return false;

  for (usize i = 0; i <= haystack.len - n; i++)
    {
      if (memcmp (haystack.data + i, s, n) == 0)
        return true;
    }

  return false;
}


bool
sv_contains_cstr (struct sv haystack, const char *s)
{
  return sv_contains_cstrn (haystack, s, strlen (s));
}


bool
sv_contains_str (struct sv haystack, struct str str)
{
  return sv_contains_cstrn (haystack, str.data, str.len);
}


bool
sv_contains_sv (struct sv haystack, struct sv sv)
{
  return sv_contains_cstrn (haystack, sv.data, sv.len);
}


// Slicing


struct sv
sv_slice (struct sv sv, usize start, usize end)
{
  struct sv sv2;

  sv2.data = sv.data + start;
  sv2.len = end - start;

  return sv2;
}


/// String Builder


struct sb
sb_from_cstrn (int *S, const char *s, usize n)
{
  struct sb sb = SB0;

  da_extend (S, &sb, s, n);

  return sb;
}


struct sb
sb_from_cstr (int *S, const char *s)
{
  return sb_from_cstrn (S, s, strlen (s));
}


struct sb
sb_from_str (int *S, struct str str)
{
  return sb_from_cstrn (S, str.data, str.len);
}


struct sb
sb_from_sv (int *S, struct sv sv)
{
  return sb_from_cstrn (S, sv.data, sv.len);
}


void
sb_to_cstrn (const struct sb *sb, char *buffer, usize n)
{
  usize size = sb->len;

  if (size > n - 1)
    size = n - 1;

  memcpy (buffer, sb->data, size);

  buffer[size] = '\0';
}


struct str
sb_into_str (struct sb *sb)
{
  struct str str;

  str.data = sb->data;
  str.len = sb->len;

  *sb = SB0;

  return str;
}


void
sb_append (int *S, struct sb *sb, char c)
{
  da_append (S, sb, c);
}


void
sb_extend_cstrn (int *S, struct sb *sb, const char *s, usize n)
{
  da_extend (S, sb, s, n);
}


void
sb_extend_cstr (int *S, struct sb *sb, const char *s)
{
  sb_extend_cstrn (S, sb, s, strlen (s));
}


void
sb_extend_str (int *S, struct sb *sb, struct str str)
{
  sb_extend_cstrn (S, sb, str.data, str.len);
}


void
sb_extend_sv (int *S, struct sb *sb, struct sv sv)
{
  sb_extend_cstrn (S, sb, sv.data, sv.len);
}


void
sb_extend_fmt (int *S, struct sb *sb, const char *fmt, ...)
{
  va_list args;
  va_list args_copy;

  va_start (args, fmt);

  va_copy (args_copy, args);

  int n = vsnprintf (NULL, 0, fmt, args_copy);

  va_end (args_copy);

  if (n < 0)
    {
      va_end (args);

      return;
    }

  usize len = sb->len;

  da_reserve (S, sb, sb->len + n);

  if (SERROR (S))
    {
      va_end (args);

      return;
    }

  sb->len += n;

  vsnprintf (sb->data + len, (usize)n + 1, fmt, args);

  va_end (args);
}


void
sb_read_file (int *S, struct sb *sb, const char *path)
{
  FILE *file = fopen (path, "r");

  if (!file)
    {
      sset (S, -1);

      return;
    }

  if (fseek (file, 0, SEEK_END) != 0)
    {
      fclose (file);

      sset (S, -1);

      return;
    }

  long nbyte = ftell (file);

  if (nbyte < 0)
    {
      fclose (file);

      sset (S, -1);

      return;
    }

  rewind (file);

  da_reserve (S, sb, (size_t)nbyte);

  if (fread (sb->data, 1, nbyte, file) != (size_t)nbyte)
    {
      sb_clear (sb);

      fclose (file);

      sset (S, -1);

      return;
    }

  sb->len = nbyte;

  if (fclose (file) != 0)
    sset (S, -1);
}



void
sb_clear (struct sb *sb)
{
  da_clear (sb);
}


/// Custom


