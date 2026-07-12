---
title: Convolution
documentation_of: ../src/algorithm/fft/convolution.hpp
---

NTT による畳み込み。

`arbitrary_mod_convolution` は 3 つの NTT-friendly prime で畳み込んだ結果を CRT で戻す。

## 関数

- `number_theoretic_transform<MOD>(a, inverse = false)`
    - NTT と逆 NTT。
- `convolution_ntt<MOD>(a, b)`
    - `MOD` が NTT 可能な場合の畳み込み。
- `arbitrary_mod_convolution<MOD>(a, b)`
    - 任意の `Modint<MOD>` に対する畳み込み。

## 計算量

- $O(n \log n)$
