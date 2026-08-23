#include <stdio.h>
#include <math.h>
#include "common.h"


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
  0.12f,
  0.27f,
  0.50f,
  0.73f,
  0.88f
};


enum
{
  N = 5,
};


f32
loss (f32 w_11, f32 b_1, f32 w_21, f32 b_2)
{
  f32 s = 0;

  for (usize i = 0; i < N; ++i)
    {
      f32 x = ti[i];
      f32 y = to[i];

      f32 a1 = sigmoid (x  * w_11 + b_1);
      f32 a2 = sigmoid (a1 * w_21 + b_2);

      f32 y_hat = a2;

      f32 d = y - y_hat;

      s += d * d;
    }

  return s / N;
}


/*
  σ(x) = 1 / (1 + e^(-x))
  σ'(x) = σ(x) * (1 - σ(x))

  a1_i = σ(x_i  * w_11 + b_1)
  a2_i = σ(a1_i * w_21 + b_2)

  ∂a2_i/∂w_21 = ∂/∂w_21 σ(a1_i * w_21 + b_2)
  ∂a2_i/∂w_21 = (a2_i * (1 - a2_i)) * ∂/∂w_21 (a1_i * w_21)
  ∂a2_i/∂w_21 = (a2_i * (1 - a2_i)) * a1_i

  ∂a2_i/∂b_2 = ∂/∂b_2 σ(a1_i * w_21 + b_2)
  ∂a2_i/∂b_2 = (a2_i * (1 - a2_i))

  ∂a2_i/∂w_11 = ∂/∂w_11 σ(a1_i * w_21 + b_2)
  ∂a2_i/∂w_11 = (a2_i * (1 - a2_i)) * ∂/∂w_11 (a1_i * w_21)
  ∂a2_i/∂w_11 = (a2_i * (1 - a2_i)) * w_21 * ((a1_i * (1 - a1_i)) * x_i)

  ∂a2_i/∂b_1 = ∂/∂b_1 σ(a1_i * w_21 + b_2)
  ∂a2_i/∂b_1 = (a2_i * (1 - a2_i)) * ∂/∂b_1 (a1_i * w_21)
  ∂a2_i/∂b_1 = (a2_i * (1 - a2_i)) * w_21 * (a1_i * (1 - a1_i))

  L = 1/n * Σ_{i=1}^n (a2_i - y_i)^2

  ∂L/∂w_21 = ∂/∂w_21 ( 1/n * Σ_{i=1}^n (a2_i - y_i)^2 )
  ∂L/∂w_21 = 1/n * Σ_{i=1}^n ∂/∂w_21 (a2_i - y_i)^2
  ∂L/∂w_21 = 1/n * Σ_{i=1}^n 2(a2_i - y_i) * ∂/∂w_21 (a2_i - y_i)
  ∂L/∂w_21 = 1/n * Σ_{i=1}^n 2(a2_i - y_i) * ((a2_i * (1 - a2_i)) * a1_i)

  ∂L/∂b_2 = 1/n * Σ_{i=1}^n 2(a2_i - y_i) * ∂/∂b_2 a2_i
  ∂L/∂b_2 = 1/n * Σ_{i=1}^n 2(a2_i - y_i) * (a2_i * (1 - a2_i))

  ∂L/∂w_11 = ∂/∂w_11 (1/n * Σ_{i=1}^n (a2_i - y_i)^2)
  ∂L/∂w_11 = 1/n * Σ_{i=1}^n ∂/∂w_11 (a2_i - y_i)^2
  ∂L/∂w_11 = 1/n * Σ_{i=1}^n 2(a2_i - y_i) * ∂/∂w_11 (a2_i - y_i)
  ∂L/∂w_11 = 1/n * Σ_{i=1}^n 2(a2_i - y_i) * (a2_i * (1 - a2_i)) * w_21 * ((a1_i * (1 - a1_i)) * x_i)

  ∂L/∂b_1 = 1/n * Σ_{i=1}^n 2(a2_i - y_i) * ∂/∂b_1 a2_i
  ∂L/∂b_1 = 1/n * Σ_{i=1}^n 2(a2_i - y_i) * (a2_i * (1 - a2_i)) * w_21 * (a1_i * (1 - a1_i))
*/


void
dloss (f32 w_11, f32 b_1, f32 w_21, f32 b_2, f32 *dw_11, f32 *db_1, f32 *dw_21, f32 *db_2)
{
  *dw_11 = 0;
  *db_1  = 0;
  *dw_21 = 0;
  *db_2  = 0;

  for (usize i = 0; i < N; ++i)
    {
      f32 x_i = ti[i];
      f32 y_i = to[i];

      f32 a1_i = sigmoid (x_i  * w_11 + b_1);
      f32 a2_i = sigmoid (a1_i * w_21 + b_2);

      // NOTE: Repetition is on purpose.
      // *dw_21 += (2.0f * (a2_i - y_i)) * (a2_i * (1.0f - a2_i)) * a1_i;
      // *db_2  += (2.0f * (a2_i - y_i)) * (a2_i * (1.0f - a2_i));
      // *dw_11 += (2.0f * (a2_i - y_i)) * (a2_i * (1.0f - a2_i)) * w_21 * (a1_i * (1.0f - a1_i)) * x_i;
      // *db_1  += (2.0f * (a2_i - y_i)) * (a2_i * (1.0f - a2_i)) * w_21 * (a1_i * (1.0f - a1_i));

      f32 delta_2 = (2.0f * (a2_i - y_i)) * (a2_i * (1.0f - a2_i));
      f32 delta_1 = delta_2 * w_21        * (a1_i * (1.0f - a1_i));

      *dw_21 += delta_2 * a1_i;
      *db_2  += delta_2;
      *dw_11 += delta_1 * x_i;
      *db_1  += delta_1;
    }

  *dw_11 /= N;
  *db_1  /= N;
  *dw_21 /= N;
  *db_2  /= N;
}


int
main (void)
{
  f32 w_11 = 0.25;
  f32 b_1 = 0.5;
  f32 w_21 = 0.75;
  f32 b_2 = 0.5;

  printf ("L=%f, w_11 = %f, b_1 = %f, w_21 = %f, b_2 = %f\n", loss (w_11, b_1, w_21, b_2), w_11, b_1, w_21, b_2);

  for (int i = 0; i < 100 * 1000; ++i)
    {
      f32 dw_11;
      f32 db_1;
      f32 dw_21;
      f32 db_2;

      dloss (w_11, b_1, w_21, b_2, &dw_11, &db_1, &dw_21, &db_2);

      w_11 -= 1e-1 * dw_11;
      b_1  -= 1e-1 * db_1;
      w_21 -= 1e-1 * dw_21;
      b_2  -= 1e-1 * db_2;
    }

  printf ("L=%f, w_11 = %f, b_1 = %f, w_21 = %f, b_2 = %f\n", loss (w_11, b_1, w_21, b_2), w_11, b_1, w_21, b_2);

  for (int i = 0; i < N; ++i)
    printf ("% .2f => %.2f\n", ti[i], sigmoid (sigmoid (ti[i] * w_11 + b_1) * w_21 + b_2));

  return 0;
}

