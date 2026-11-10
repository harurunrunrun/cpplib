---
title: Min-Plus Convolution by SMAWK (SMAWK法による最小加算畳み込み) [min_plus_convolution_convex_arbitrary]
documentation_of: ../../../../src/algorithm/fft/min_plus/min_plus_convolution_smawk.hpp
---

離散凸な符号付き整数列と任意の符号付き整数列の min-plus 畳み込みを SMAWK 法で計算する。凸列の範囲外を十分大きいペナルティで延長して全体を totally monotone な行列にし、`smawk_row_minima` で各行の最小値を求める。

## `min_plus_convolution_convex_arbitrary_smawk`

```cpp
template<std::signed_integral T>
std::vector<T> min_plus_convolution_convex_arbitrary_smawk(
    const std::vector<T>& convex,
    const std::vector<T>& arbitrary
)
```

長さ `convex.size() + arbitrary.size() - 1` の列

$$
c_k=\min_{i+j=k}(\mathtt{convex}_i+\mathtt{arbitrary}_j)
$$

を返す。どちらかの入力が空なら空列を返す。`convex` の離散凸性は実行時に検査する。

## 計算量

$N = $ `convex.size()`、$M = $ `arbitrary.size()` とする。

- 時間計算量: $O(N + M)$
- 空間計算量: $O(N + M)$

## 注意点

- `T` は 64 bit 以下の符号付き整数型でなければならない（`static_assert` で検査する）。
- 第 1 引数が離散凸でなければ `std::invalid_argument` を送出する。
- 入出力の添字を `int` で表せなければ `std::length_error` を送出する。
- 候補の比較は `__int128_t` で行う。最小値が `T` の範囲外なら `std::overflow_error` を送出する。
