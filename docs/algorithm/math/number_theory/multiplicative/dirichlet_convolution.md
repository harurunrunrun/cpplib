---
title: Dirichlet Convolution (Dirichlet畳み込み)
documentation_of: ../../../../../src/algorithm/math/number_theory/multiplicative/dirichlet_convolution.hpp
---

数論的関数のディリクレ畳み込み $(f \ast g)(n) = \sum_{d \mid n} f(d) g(n/d)$ を、$1$ から $N$ までの値を並べた表の上で計算する。

## `math::dirichlet_convolution(first, second)`

`std::vector<Value>` で与えた 2 つの表の添字 $1$ から $N$ までを使い、添字 $0$ は無視する。戻り値の添字 $n$ には $\sum_{d \mid n} \mathtt{first}[d] \, \mathtt{second}[n/d]$ を格納する（添字 $0$ はデフォルト値）。

## `math::dirichlet_inverse(values)`

`values[1]` が可逆な関数のディリクレ逆元を返す。戻り値を `inverse` とすると、`dirichlet_convolution(values, inverse)` は添字 $1$ だけが $1$ で、ほかが $0$ になる。`inverse[q]` を小さい $q$ から順に確定させ、その寄与をすべての倍数 $dq$ へ伝播して求める。

## 時間計算量・空間計算量

- `dirichlet_convolution`: 時間 $O(N \log N)$、空間 $O(N)$。
- `dirichlet_inverse`: 時間 $O(N \log N)$、空間 $O(N)$。

どちらも積和の回数は $\sum_{q=1}^{N} \lfloor N/q \rfloor = O(N \log N)$ である。

## 注意点

- 入力の添字 $0$ は参照しない。
- `dirichlet_convolution` の 2 つの入力は同じ正の長さでなければならない。`dirichlet_inverse` の入力は添字 $1$ を含む長さ（$2$ 以上）でなければならない。違反時は `std::invalid_argument` を送出する。
- `Value` はデフォルト値を加法単位元、`Value{1}` を乗法単位元として扱えなければならず、加算・乗算・除算・単項マイナスを提供する必要がある。`values[1]` は `Value` 上で可逆でなければならない。これらの代数的な前提は実行時には検査しない。
