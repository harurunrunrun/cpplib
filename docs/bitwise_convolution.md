---
title: Bitwise Convolution
documentation_of: ../src/algorithm/fft/bitwise_convolution.hpp
---

bit 演算に対する畳み込み。

入力長は同じ 2 冪。

## 関数

- `or_zeta_transform(a)`, `or_mobius_transform(a)`
    - OR ゼータ変換と逆変換。
- `and_zeta_transform(a)`, `and_mobius_transform(a)`
    - AND ゼータ変換と逆変換。
- `walsh_hadamard_transform(a, inverse = false)`
    - XOR 用の Walsh-Hadamard 変換。
- `or_convolution(a, b)`
    - `i | j = k` となる積和。
- `and_convolution(a, b)`
    - `i & j = k` となる積和。
- `xor_convolution(a, b)`
    - `i ^ j = k` となる積和。
- `xnor_convolution(a, b)`
    - `~(i ^ j) = k` となる積和。ただし反転は入力長に対応する bit 幅で行う。

## 計算量

- $O(n \log n)$
