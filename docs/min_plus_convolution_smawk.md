---
title: Min-Plus Convolution by SMAWK (SMAWK法による最小加算畳み込み) [min_plus_convolution_convex_arbitrary]
documentation_of: ../src/algorithm/fft/min_plus_convolution_smawk.hpp
---

離散凸な符号付き整数列と任意の符号付き整数列のmin-plus convolutionを
SMAWK法で計算する。

## `min_plus_convolution_convex_arbitrary_smawk`

```cpp
template<std::signed_integral T>
vector<T> min_plus_convolution_convex_arbitrary_smawk(
    const vector<T>& convex,
    const vector<T>& arbitrary
)
```

次の長さ `convex.size() + arbitrary.size() - 1` の列を返す。

$$
c_k=\min_{i+j=k}(\mathtt{convex}_i+\mathtt{arbitrary}_j)
$$

どちらかの入力が空なら空列を返す。`convex` の離散凸性は実行時に検査する。

## 時間計算量

$N=$ `convex.size()`、$M=$ `arbitrary.size()` として $O(N+M)$。

## 空間計算量

$O(N+M)$。

## 注意点

- `T` は64bit以下の符号付き整数型でなければならない。
- 第1引数が離散凸でなければ `invalid_argument`。
- 入出力indexを `int` で表せなければ `length_error`。
- 候補比較は `__int128_t` で行う。最小値が `T` の範囲外なら
  `overflow_error`。
