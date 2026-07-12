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
