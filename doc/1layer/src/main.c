#include <stdio.h>
#include "common.h"


f32 ti[] = {
  0,
  1,
  2,
  3,
  4,
};


f32 to[] = {
  0,
  2,
  4,
  6,
  8,
};


enum
{
  N = 5,
};


f32
loss (f32 w)
{
  f32 s = 0;

  for (usize i = 0; i < N; ++i)
    {
      f32 x = ti[i];
      f32 y = to[i];

      f32 y_hat = x * w;

      f32 d = y - y_hat;

      s += d * d;
    }

  return s / N;
}


/*
  L(w) = 1/n * Σ_{i=1}^n (x_{i} * w - y_{i})^2

  dL/dw = d/dw ( 1/n * Σ_{i=1}^n (x_{i} * w - y_{i})^2 )
  dL/dw = 1/n * Σ_{i=1}^n d/dw (x_{i} * w - y_{i})^2
  dL/dw = 1/n * Σ_{i=1}^n 2(x_{i} * w - y_{i}) * d/dw (x_{i} * w - y_{i})
  dL/dw = 1/n * Σ_{i=1}^n 2(x_{i} * w - y_{i}) * x_{i} * d/dw (w)
  dL/dw = 1/n * Σ_{i=1}^n 2(x_{i} * w - y_{i}) * x_{i}
*/


f32
dloss (f32 w)
{
  f32 s = 0;

  for (usize i = 0; i < N; ++i)
    {
      f32 x_i = ti[i];
      f32 y_i = to[i];

      s += 2 * (x_i * w - y_i) * x_i;
    }

  return s / N;
}


int
main (void)
{
  f32 w = 0;

  printf ("L=%f, w=%f\n", loss (w), w);

  for (int i = 0; i < 100; ++i)
    {
      w -= 1e-1 * dloss (w);
    }

  printf ("L=%f, w=%f\n", loss (w), w);

  printf ("%.2f => %.2f\n", 0.0f, 0.0f * w);
  printf ("%.2f => %.2f\n", 1.0f, 1.0f * w);
  printf ("%.2f => %.2f\n", 2.0f, 2.0f * w);
  printf ("%.2f => %.2f\n", 3.0f, 3.0f * w);
  printf ("%.2f => %.2f\n", 4.0f, 4.0f * w);

  return 0;
}

