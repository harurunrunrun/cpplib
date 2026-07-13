---
title: FFT
documentation_of: ../src/algorithm/fft/fft.hpp
---

複素数 FFT。

## 関数

- `fast_fourier_transform(a, inverse = false)`
    - FFT と逆 FFT。入力長は 2 冪。
- `convolution_fft(a, b)`
    - `double` 列の畳み込み。
- `convolution_ll(a, b)`
    - `long long` 列の畳み込み。結果は丸める。

## 計算量

- $O(n \log n)$

## API別計算量

$A=|a|$, $B=|b|$、$Z$ を $A+B-1$ 以上の最小の2冪とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `fast_fourier_transform(a,inverse)` | $O(N\log N)$ | 追加 $O(1)$ |
| `convolution_fft(a,b)` | $O(Z\log Z)$ | $O(Z)$ |
| `convolution_ll(a,b)` | $O(Z\log Z)$ | $O(Z)$ |

畳み込みの戻り値列挙は $O(A+B)$ で、上表の時間に含まれる。`convolution_ll` はFFT結果の全要素を丸める $O(A+B)$ 処理も含む。
