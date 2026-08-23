#ifndef MATRIX_H
#define MATRIX_H


#include "common.h"


typedef struct
{
  usize r;
  usize c;
  usize s;
  f32 *data;
} matrix;


#define matrix_at(m, i, j) (m).data[(i) * (m).s + (j)]


#define matrix_create_row(S, m, c) matrix_create ((S), (m), 1, (c))
#define matrix_create_col(S, m, r) matrix_create ((S), (m), (r), 1)


void
matrix_create (int *S, matrix *m, usize r, usize c);

void
matrix_destroy (matrix *m);


void
matrix_copy (matrix m, matrix a);


void
matrix_fill (matrix m, f32 x);

void
matrix_fill_with (matrix m, f32 *data);

void
matrix_fill_random (matrix m, f32 lo, f32 hi);


matrix
matrix_view_col (matrix m, usize c);

matrix
matrix_view_row (matrix m, usize r);


void
matrix_transpose (matrix d, matrix a);


void
matrix_add (matrix d, matrix a, matrix b);

void
matrix_sub (matrix d, matrix a, matrix b);

void
matrix_subs_l (matrix d, f32 s, matrix a);

void
matrix_dot (matrix d, matrix a, matrix b);

// "dot (d, transpose (a), b)"
void
matrix_transpose_dot_l (matrix d, matrix a, matrix b);

// "dot (d, a, transpose (b))"
void
matrix_transpose_dot_r (matrix d, matrix a, matrix b);

void
matrix_mul (matrix d, matrix a, matrix b);

void
matrix_muls (matrix d, matrix a, f32 s);


void
matrix_map (matrix d, matrix a, f32 (*f) (f32));


void
matrix_dump_indent (matrix m, int indent);

void
matrix_dump_prefix (matrix m, const char *fmt, ...);

void
matrix_dump_indent_prefix (matrix m, int indent, const char *fmt, ...);


#define matrix_dump_e(m) matrix_dump_prefix ((m), #m " = ")

#define matrix_dump_indent_e(m, indent) matrix_dump_indent_prefix ((m), (indent), #m " = ")


void
matrix_dump (matrix m);


#endif // MATRIX_H

