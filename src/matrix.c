#include "matrix.h"
#include <math.h>


void
matrix_create (int *S, matrix *m, usize r, usize c)
{
  m->r = r;
  m->c = c;
  m->s = c;

  m->data = allocate0_n (S, r * c, sizeof (f32));
}


void
matrix_destroy (matrix *m)
{
  deallocate (m->data);

  m->r = 0;
  m->c = 0;
  m->s = 0;
}


void
matrix_copy (matrix m, matrix a)
{
  panic_assert (m.r == a.r);
  panic_assert (m.c == a.c);

  for (usize i = 0; i < m.r; ++i)
    for (usize j = 0; j < m.c; ++j)
      matrix_at (m, i, j) = matrix_at (a, i, j);
}


void
matrix_fill (matrix m, f32 x)
{
  for (usize i = 0; i < m.r; ++i)
    for (usize j = 0; j < m.c; ++j)
      matrix_at (m, i, j) = x;
}


void
matrix_fill_with (matrix m, f32 *data)
{
  memcpy (m.data, data, m.r * m.c * sizeof (f32));
}


void
matrix_fill_random (matrix m, f32 lo, f32 hi)
{
  for (usize i = 0; i < m.r; ++i)
    for (usize j = 0; j < m.c; ++j)
      matrix_at (m, i, j) = random_f32 (lo, hi);
}


matrix
matrix_view_col (matrix m, usize c)
{
  matrix n;

  n.r = m.r;
  n.c = 1;
  n.s = m.s;

  n.data = m.data + c;

  return n;
}


matrix
matrix_view_row (matrix m, usize r)
{
  matrix n;

  n.r = 1;
  n.c = m.c;
  n.s = m.s;

  n.data = m.data + r * m.s;

  return n;
}


void
matrix_transpose (matrix d, matrix a)
{
  panic_assert (d.r == a.c);
  panic_assert (d.c == a.r);

  for (usize i = 0; i < d.r; ++i)
    for (usize j = 0; j < d.c; ++j)
      matrix_at (d, i, j) = matrix_at (a, j, i);
}


void
matrix_add (matrix d, matrix a, matrix b)
{
  panic_assert (a.r == b.r);
  panic_assert (a.c == b.c);

  panic_assert (d.r == a.r);
  panic_assert (d.c == a.c);

  for (usize i = 0; i < d.r; ++i)
    for (usize j = 0; j < d.c; ++j)
      matrix_at (d, i, j) = matrix_at (a, i, j) + matrix_at (b, i, j);
}


void
matrix_sub (matrix d, matrix a, matrix b)
{
  panic_assert (a.r == b.r);
  panic_assert (a.c == b.c);

  panic_assert (d.r == a.r);
  panic_assert (d.c == a.c);

  for (usize i = 0; i < d.r; ++i)
    for (usize j = 0; j < d.c; ++j)
      matrix_at (d, i, j) = matrix_at (a, i, j) - matrix_at (b, i, j);
}


void
matrix_subs_l (matrix d, f32 s, matrix a)
{
  panic_assert (d.r == a.r);
  panic_assert (d.c == a.c);

  for (usize i = 0; i < d.r; ++i)
    for (usize j = 0; j < d.c; ++j)
      matrix_at (d, i, j) = s - matrix_at (a, i, j);
}


void
matrix_dot (matrix d, matrix a, matrix b)
{
  panic_assert (a.c == b.r);

  panic_assert (d.r == a.r);
  panic_assert (d.c == b.c);

  const usize N = a.c;

  for (usize i = 0; i < d.r; ++i)
    for (usize j = 0; j < d.c; ++j)
      {
        f32 x = 0;

        for (usize k = 0; k < N; ++k)
          x += matrix_at (a, i, k) * matrix_at (b, k, j);

        matrix_at (d, i, j) = x;
      }
}


void
matrix_transpose_dot_l (matrix d, matrix a, matrix b)
{
  panic_assert (a.r == b.r);

  panic_assert (d.r == a.c);
  panic_assert (d.c == b.c);

  const usize N = a.r;

  for (usize i = 0; i < d.r; ++i)
    for (usize j = 0; j < d.c; ++j)
      {
        f32 x = 0;

        for (usize k = 0; k < N; ++k)
          x += matrix_at (a, k, i) * matrix_at (b, k, j);

        matrix_at (d, i, j) = x;
      }
}


void
matrix_transpose_dot_r (matrix d, matrix a, matrix b)
{
  panic_assert (a.c == b.c);

  panic_assert (d.r == a.r);
  panic_assert (d.c == b.r);

  const usize N = a.c;

  for (usize i = 0; i < d.r; ++i)
    for (usize j = 0; j < d.c; ++j)
      {
        f32 x = 0;

        for (usize k = 0; k < N; ++k)
          x += matrix_at (a, i, k) * matrix_at (b, j, k);

        matrix_at (d, i, j) = x;
      }
}


void
matrix_mul (matrix d, matrix a, matrix b)
{
  panic_assert (a.r == b.r);
  panic_assert (a.c == b.c);

  panic_assert (d.r == a.r);
  panic_assert (d.c == a.c);

  for (usize i = 0; i < d.r; ++i)
    for (usize j = 0; j < d.c; ++j)
      matrix_at (d, i, j) = matrix_at (a, i, j) * matrix_at (b, i, j);
}


void
matrix_muls (matrix d, matrix a, f32 s)
{
  panic_assert (d.r == a.r);
  panic_assert (d.c == a.c);

  for (usize i = 0; i < d.r; ++i)
    for (usize j = 0; j < d.c; ++j)
      matrix_at (d, i, j) = matrix_at (a, i, j) * s;
}


void
matrix_map (matrix d, matrix a, f32 (*f) (f32))
{
  panic_assert (d.r == a.r);
  panic_assert (d.c == a.c);

  for (usize i = 0; i < d.r; ++i)
    for (usize j = 0; j < d.c; ++j)
      matrix_at (d, i, j) = f (matrix_at (a, i, j));
}


#define MATRIX_DUMP_PRECISION "4"


void
matrix_dump_indent (matrix m, int indent)
{
  if (m.r == 0)
    {
      printf ("[]\n");

      return;
    }

  int indents[m.c];

  for (usize j = 0; j < m.c; ++j)
    {
      int w_col = 0;

      for (usize r = 0; r < m.r; ++r)
        w_col = max_int (w_col, snprintf (NULL, 0, "%." MATRIX_DUMP_PRECISION "f", matrix_at (m, r, j)));

      indents[j] = w_col;
    }

  for (usize i = 0; i < m.r; ++i)
    {
      // L Bracket
      if (m.r == 1)
        printf ("[ ");
      else
        {
          if (i != 0)
            printf ("%*s", indent, "");

          if (i == 0)
            printf ("⎡ ");

          else if (i == m.r - 1)
            printf ("⎣ ");

          else
            printf ("⎢ ");
        }

      // Data
      for (usize j = 0; j < m.c; ++j)
        {
          if (j > 0)
            printf (" ");

          printf ("%*." MATRIX_DUMP_PRECISION "f", indents[j], matrix_at (m, i, j));
        }

      // R Bracket
      if (m.r == 1)
        printf (" ]");
      else
        {
          if (i == 0)
            printf (" ⎤");

          else if (i == m.r - 1)
            printf (" ⎦");

          else
            printf (" ⎥");
        }

      printf ("\n");
    }
}


void
matrix_dump_prefix (matrix m, const char *fmt, ...)
{
  va_list va;

  va_start (va, fmt);

  int n = vprintf (fmt, va);

  va_end (va);

  matrix_dump_indent (m, n);
}


void
matrix_dump_indent_prefix (matrix m, int indent, const char *fmt, ...)
{
  indent_dump (indent);

  va_list va;

  va_start (va, fmt);

  int n = vprintf (fmt, va);

  va_end (va);

  matrix_dump_indent (m, n + indent);
}


void
matrix_dump (matrix m)
{
  matrix_dump_indent (m, 0);
}

