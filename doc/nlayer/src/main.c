#include <stdio.h>
#include <math.h>
#include <time.h>
#include "common.h"


f32
random_f32 (void)
{
  return (f32)rand () / RAND_MAX;
}


f32
sigmoid (f32 x)
{
  return 1.0f / (1.0f + exp (-x));
}


f32 ti[] = {
  -2.0f,
  -1.0f,
   0.0f,
   1.0f,
   2.0f
};


f32 to[] = {
  0.1f,
  0.2f,
  0.5f,
  0.8f,
  0.9f
};


enum
{
  N = 5,
};


enum
{
  L = 5,
};


f32
forward (f32 ws[L], f32 bs[L], f32 x, f32 as[L])
{
  for (int l = 0; l < L; ++l)
    {
      x = sigmoid (x * ws[l] + bs[l]);

      if (as != NULL)
        as[l] = x;
    }

  return x;
}


f32
loss (f32 ws[L], f32 bs[L])
{
  f32 s = 0;

  for (usize i = 0; i < N; ++i)
    {
      f32 x = ti[i];
      f32 y = to[i];

      f32 y_hat = forward (ws, bs, x, NULL);

      f32 d = y - y_hat;

      s += d * d;
    }

  return s / N;
}


void
dloss (f32 ws[L], f32 bs[L], f32 dws[L], f32 dbs[L])
{
  for (usize l = 0; l < L; ++l)
    {
      dws[l] = 0;
      dbs[l] = 0;
    }

  for (usize i = 0; i < N; ++i)
    {
      f32 x_i = ti[i];
      f32 y_i = to[i];

      f32 as[L];

      forward (ws, bs, x_i, as);

      f32 delta = (2.0f * (as[L - 1] - y_i)) * (as[L - 1] * (1.0f - as[L - 1]));

      for (usize l = L; l-- > 0;)
        {
          if (l != L - 1)
            delta = delta * ws[l + 1] * (as[l] * (1.0f - as[l]));

          f32 x;
          if (l == 0)
            x = x_i;
          else
            x = as[l - 1];

          dws[l] += delta * x;
          dbs[l] += delta;
        }
    }

  for (usize l = 0; l < L; ++l)
    {
      dws[l] /= N;
      dbs[l] /= N;
    }
}


int
main (void)
{
  srand (1234);

  f32 ws[L];
  f32 bs[L];

  for (int l = 0; l < L; ++l)
    {
      ws[l] = random_f32 ();
      bs[l] = random_f32 ();
    }

  f32 dws[L];
  f32 dbs[L];

  printf ("L=%f\n", loss (ws, bs));

  for (int i = 0; i < 1000 * 1000; ++i)
    {
      dloss (ws, bs, dws, dbs);

      for (usize l = 0; l < L; ++l)
        {
          ws[l] -= 1e-1 * dws[l];
          bs[l] -= 1e-1 * dbs[l];
        }
    }

  printf ("L=%f\n", loss (ws, bs));

  for (int i = 0; i < N; ++i)
    printf ("f % .2f = %.4f\n", ti[i], forward (ws, bs, ti[i], NULL));

  return 0;
}

