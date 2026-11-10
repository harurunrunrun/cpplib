---
title: Cyclic Correlation over 64 Bit Rings (64 bit 環・体の循環相関)
documentation_of: ../../../../src/algorithm/fft/transform/cyclic_correlation_64.hpp
---

同じ長さ $N$ の列 $A, B$ について、$C_k = \sum_i A_i B_{(i+k) \bmod N}$ を返す。

```cpp
std::vector<std::uint64_t> cyclic_correlation_mod_2_64(const std::vector<std::uint64_t>& first, const std::vector<std::uint64_t>& second);
std::vector<std::uint64_t> cyclic_correlation_gf2_64(const std::vector<std::uint64_t>& first, const std::vector<std::uint64_t>& second);
```

`cyclic_correlation_mod_2_64` は法 $2^{64}$ の環で、`cyclic_correlation_gf2_64` は体 $\mathrm{GF}(2^{64})$ で計算する。体は既約多項式 $x^{64}+x^4+x^3+x+1$ による多項式基底で表し、和は XOR になる。

## 計算量

第 1 引数を循環的に反転して `convolution_mod_2_64` または `gf2_64_convolution` に渡し、次数 $N$ 以上の項を折り返して足す。通常の畳み込み 1 回と $O(N)$ の追加処理なので、時間計算量は各畳み込みと同じオーダーであり、追加の空間計算量は $O(N)$ である。

## 注意点

- GCC 13 / C++20 を想定する。
- 2 つの列の長さが異なる場合は `std::invalid_argument` を送出する。空列同士の相関は空列である。
- 内部の 64 bit 畳み込みと同じ長さ上限 $N \le 524288$ が適用される（超えると `std::length_error`）。
