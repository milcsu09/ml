#include <stdio.h>
#include <math.h>
#include <time.h>
#include "common.h"
#include "matrix.h"


f32
sigmoid (f32 x)
{
  return 1.0f / (1.0f + expf (-x));
}


struct nn
{
  // Forward
  matrix *ws;
  matrix *bs;

  // Activations
  matrix *as;

  // Backward
  matrix *dws;
  matrix *dbs;

  // Temporary
  matrix *delta;
  matrix *delta_f;
  matrix *delta_a;

  usize L;
  usize K;
};


#define NN_LAYERS(...)                                                                             \
  (usize[]){ __VA_ARGS__ }, sizeof (usize[]){ __VA_ARGS__ } / sizeof (usize)


void
nn_create (int *S, struct nn *nn, usize *layers, usize L)
{
  panic_assert (L >= 2);

  nn->L = L;
  nn->K = L - 1;

  nn->ws = allocate0_n (S, nn->K, sizeof (matrix));
  nn->bs = allocate0_n (S, nn->K, sizeof (matrix));

  nn->as = allocate0_n (S, nn->L, sizeof (matrix));

  nn->dws = allocate0_n (S, nn->K, sizeof (matrix));
  nn->dbs = allocate0_n (S, nn->K, sizeof (matrix));

  nn->delta   = allocate0_n (S, nn->K, sizeof (matrix));
  nn->delta_f = allocate0_n (S, nn->K, sizeof (matrix));
  nn->delta_a = allocate0_n (S, nn->K, sizeof (matrix));

  if (SERROR (S))
    goto clean_nn;

  matrix_create_col (S, &nn->as[0], layers[0]);

  if (SERROR (S))
    goto clean_nn;

  for (usize l = 1; l < nn->L; ++l)
    {
      const usize k = l - 1;

      matrix_create (S, &nn->ws[k], layers[l], nn->as[l - 1].r);
      matrix_create (S, &nn->bs[k], layers[l], 1);

      matrix_create (S, &nn->as[l], layers[l], 1);

      matrix_create (S, &nn->dws[k], layers[l], nn->as[l - 1].r);
      matrix_create (S, &nn->dbs[k], layers[l], 1);

      matrix_create (S, &nn->delta[k],   nn->as[l].r, nn->as[l].c);
      matrix_create (S, &nn->delta_f[k], nn->as[l].r, nn->as[l].c);
      matrix_create (S, &nn->delta_a[k], nn->dws[k].r, nn->dws[k].c);

      if (SERROR (S))
        goto clean_nn;
    }

  return;

clean_nn:
  if (nn->ws)
    for (usize k = 0; k < nn->K; ++k)
      matrix_destroy (&nn->ws[k]);

  if (nn->bs)
    for (usize k = 0; k < nn->K; ++k)
      matrix_destroy (&nn->bs[k]);

  if (nn->as)
    for (usize l = 0; l < nn->L; ++l)
      matrix_destroy (&nn->as[l]);

  if (nn->dws)
    for (usize k = 0; k < nn->K; ++k)
      matrix_destroy (&nn->dws[k]);

  if (nn->dbs)
    for (usize k = 0; k < nn->K; ++k)
      matrix_destroy (&nn->dbs[k]);

  if (nn->delta)
    for (usize k = 0; k < nn->K; ++k)
      matrix_destroy (&nn->delta[k]);

  if (nn->delta_f)
    for (usize k = 0; k < nn->K; ++k)
      matrix_destroy (&nn->delta_f[k]);

  if (nn->delta_a)
    for (usize k = 0; k < nn->K; ++k)
      matrix_destroy (&nn->delta_a[k]);

  deallocate (nn->ws);
  deallocate (nn->bs);
  deallocate (nn->as);
  deallocate (nn->dws);
  deallocate (nn->dbs);
  deallocate (nn->delta);
  deallocate (nn->delta_f);
  deallocate (nn->delta_a);
}


void
nn_destroy (struct nn *nn)
{
  matrix_destroy (&nn->as[0]);

  for (usize l = 1; l < nn->L; ++l)
    {
      const usize k = l - 1;

      matrix_destroy (&nn->ws[k]);
      matrix_destroy (&nn->bs[k]);
      matrix_destroy (&nn->as[l]);
      matrix_destroy (&nn->dws[k]);
      matrix_destroy (&nn->dbs[k]);
      matrix_destroy (&nn->delta[k]);
      matrix_destroy (&nn->delta_f[k]);
      matrix_destroy (&nn->delta_a[k]);
    }

  deallocate (nn->ws);
  deallocate (nn->bs);
  deallocate (nn->as);
  deallocate (nn->dws);
  deallocate (nn->dbs);
  deallocate (nn->delta);
  deallocate (nn->delta_f);
  deallocate (nn->delta_a);
}


matrix *
nn_input (struct nn nn)
{
  return &nn.as[0];
}


matrix *
nn_output (struct nn nn)
{
  return &nn.as[nn.L - 1];
}


void
nn_fill_random (struct nn nn, f32 lo, f32 hi)
{
  for (usize l = 1; l < nn.L; ++l)
    {
      const usize k = l - 1;

      matrix_fill_random (nn.ws[k], lo, hi);
      matrix_fill_random (nn.bs[k], lo, hi);
    }
}


#define NN_DUMP_DEBUG 0


void
nn_dump (struct nn nn)
{
  printf ("{\n");

#if NN_DUMP_DEBUG
  matrix_dump_indent_prefix (nn.as[0], 2, "     as0 = ");
  printf ("\n");
#endif

  for (usize l = 1; l < nn.L; ++l)
    {
      const usize k = l - 1;

      if (l > 1)
        printf ("\n");

      matrix_dump_indent_prefix (nn.ws[k],  2, "ws%d = ", l);
      matrix_dump_indent_prefix (nn.bs[k],  2, "bs%d = ", l);

#if NN_DUMP_DEBUG
      printf ("\n");
      matrix_dump_indent_prefix (nn.as[l],      2, "     as%d = ", l);
      matrix_dump_indent_prefix (nn.dws[k],     2, "    dws%d = ", l);
      matrix_dump_indent_prefix (nn.dbs[k],     2, "    dbs%d = ", l);
      matrix_dump_indent_prefix (nn.delta[k],   2, "  delta%d = ", l);
      matrix_dump_indent_prefix (nn.delta_f[k], 2, "delta_f%d = ", l);
      matrix_dump_indent_prefix (nn.delta_a[k], 2, "delta_a%d = ", l);
#endif
    }

  printf ("}\n");
}


matrix
nn_forward (struct nn nn, matrix x)
{
  matrix_copy (*nn_input (nn), x);

  for (usize l = 1; l < nn.L; ++l)
    {
      const usize k = l - 1;

      // z = w * x + b
      matrix_dot (nn.as[l], nn.ws[k], nn.as[l - 1]);
      matrix_add (nn.as[l], nn.as[l], nn.bs[k]);

      // a = f z
      matrix_map (nn.as[l], nn.as[l], sigmoid);
    }

  return *nn_output (nn);
}


void
nn_backward (struct nn nn, matrix ti, matrix to)
{
  panic_assert (ti.c == to.c);

  const usize N = ti.c;

  for (usize l = nn.L - 1; l > 0; --l)
    {
      const usize k = l - 1;

      matrix_fill (nn.dws[k], 0.0f);
      matrix_fill (nn.dbs[k], 0.0f);
    }

  for (usize i = 0; i < N; ++i)
    {
      matrix x = matrix_view_col (ti, i);
      matrix y = matrix_view_col (to, i);

      matrix y_hat = nn_forward (nn, x);

      panic_assert (y.r == y_hat.r);
      panic_assert (y.c == y_hat.c);

      // 2(ŷ - y)
      matrix_sub  (nn.delta[nn.K - 1], y_hat,              y);
      matrix_muls (nn.delta[nn.K - 1], nn.delta[nn.K - 1], 2.0f);

      // (ACTIVATION FUNCTION) = ŷ(1 - ŷ) = σ'
      matrix_subs_l (nn.delta_f[nn.K - 1], 1.0f,                 y_hat);
      matrix_mul    (nn.delta_f[nn.K - 1], nn.delta_f[nn.K - 1], y_hat);

      // delta_{L} = 2(ŷ - y) * (ACTIVATION FUNCTION)
      matrix_mul (nn.delta[nn.K - 1], nn.delta[nn.K - 1], nn.delta_f[nn.K - 1]);

      for (usize l = nn.L - 1; l > 0; --l)
        {
          const usize k = l - 1;

          if (l != nn.L - 1)
            {
              // (W_{l+1})^T @ delta_{l+1}
              matrix_transpose_dot_l (nn.delta[k], nn.ws[k + 1], nn.delta[k + 1]);

              // (ACTIVATION FUNCTION) = a_{l}(1 - a_{l}) = σ'
              matrix_subs_l (nn.delta_f[k], 1.0f,          nn.as[l]);
              matrix_mul    (nn.delta_f[k], nn.delta_f[k], nn.as[l]);

              // delta_{l} = (w_{l+1})^T @ delta_{l+1} * (ACTIVATION FUNCTION)
              matrix_mul (nn.delta[k], nn.delta[k],  nn.delta_f[k]);
            }

          matrix_transpose_dot_r (nn.delta_a[k], nn.delta[k], nn.as[l - 1]);

          // Update dws and dbs.
          matrix_add (nn.dws[k], nn.dws[k], nn.delta_a[k]);
          matrix_add (nn.dbs[k], nn.dbs[k], nn.delta[k]);
        }
    }

  for (usize l = nn.L - 1; l > 0; --l)
    {
      const usize k = l - 1;

      matrix_muls (nn.dws[k], nn.dws[k], 1.0f / N);
      matrix_muls (nn.dbs[k], nn.dbs[k], 1.0f / N);
    }
}


void
nn_apply (struct nn nn, f32 eta)
{
  for (usize l = nn.L - 1; l > 0; --l)
    {
      const usize k = l - 1;

      matrix_muls (nn.dws[k], nn.dws[k], eta);
      matrix_muls (nn.dbs[k], nn.dbs[k], eta);

      matrix_sub (nn.ws[k], nn.ws[k], nn.dws[k]);
      matrix_sub (nn.bs[k], nn.bs[k], nn.dbs[k]);
    }
}


f32
nn_loss (struct nn nn, matrix ti, matrix to)
{
  panic_assert (ti.c == to.c);

  const usize N = ti.c;

  f32 s = 0.0f;

  for (usize i = 0; i < N; ++i)
    {
      matrix x = matrix_view_col (ti, i);
      matrix y = matrix_view_col (to, i);

      matrix y_hat = nn_forward (nn, x);

      panic_assert (y.r == y_hat.r);
      panic_assert (y.c == y_hat.c);

      for (usize j = 0; j < y.r; ++j)
        {
          f32 d = matrix_at (y, j, 0) - matrix_at (y_hat, j, 0);

          s += d * d;
        }
    }

  return s / N;
}


int
main (void)
{
  /// CONFIG
  srand (time (0));

#define LAYERS 2, 2, 1

#define EPOCHS (35 * 1000)
#define LEARNR (1.0)

  matrix ti;
  matrix to;

  matrix_create (NULL, &ti, 2, 4);
  matrix_create (NULL, &to, 1, 4);

  matrix_fill_with (ti, (f32[]){
    0, 1, 0, 1,
    0, 0, 1, 1,
  });

  matrix_fill_with (to, (f32[]){
    0, 1, 1, 0,
  });

  /// Neural Network
  struct nn nn;

  nn_create (NULL, &nn, NN_LAYERS (LAYERS));

  nn_fill_random (nn, -1, 1);

  /// Training
  printf ("L=%f\n", nn_loss (nn, ti, to));

  for (int epoch = 0; epoch < EPOCHS; ++epoch)
    {
      nn_backward (nn, ti, to);
      nn_apply (nn, LEARNR);

      if (epoch % 1000 == 0)
        printf ("%8d (%3.0f%%), L=%f\n", epoch, ((f32)epoch / (f32)EPOCHS) * 100, nn_loss (nn, ti, to));
    }

  nn_dump (nn);
  printf ("L=%f\n\n", nn_loss (nn, ti, to));

  /// Result
  for (usize i = 0; i < ti.c; ++i)
    {
      matrix x = matrix_view_col (ti, i);
      matrix y = matrix_view_col (to, i);

      matrix y_hat = nn_forward (nn, x);

      matrix_dump (y_hat);
    }

  nn_destroy (&nn);

  matrix_destroy (&ti);
  matrix_destroy (&to);

  return 0;
}

