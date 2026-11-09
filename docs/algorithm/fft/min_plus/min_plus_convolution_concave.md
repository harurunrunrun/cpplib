---
title: Min-Plus Convolution (Concave and Arbitrary) (凹列と任意列の最小加算畳み込み) [min_plus_convolution_concave_arbitrary]
documentation_of: ../../../../src/algorithm/fft/min_plus/min_plus_convolution_concave.hpp
---

符号付き整数列の一方が離散凹であるとき、min-plus 畳み込み

$$
c_k=\min_{i+j=k}(a_i+b_j)
$$

を計算する。離散凹とは、すべての内部の添字 $i$ で

$$
a_{i-1}+a_{i+1}\leq 2a_i
$$

が成り立つこと、すなわち隣接差分が広義単調減少であることをいう。

以下、$N$ を凹列、$M$ を任意列の長さとする。

## `is_discrete_concave`

```cpp
template<std::signed_integral T>
bool is_discrete_concave(const std::vector<T>& values)
```

`values` が離散凹なら `true` を返す。長さ $0, 1, 2$ の列に対しては常に `true` を返す。

- 時間計算量: $O(N)$
- 追加の空間計算量: $O(1)$

## `min_plus_convolution_concave_arbitrary`

```cpp
template<std::signed_integral T>
std::vector<T> min_plus_convolution_concave_arbitrary(
    const std::vector<T>& concave_values,
    const std::vector<T>& arbitrary_values
)
```

第 1 引数が離散凹である場合の min-plus 畳み込みを返す。凹性は実行時に検査し、満たさなければ `std::invalid_argument` を送出する。どちらかが空なら空列を返す。

- 時間計算量: $O((N+M)\log^2(N+M))$
- 追加の空間計算量: $O(N+M)$

## `min_plus_convolution_arbitrary_concave`

```cpp
template<std::signed_integral T>
std::vector<T> min_plus_convolution_arbitrary_concave(
    const std::vector<T>& arbitrary_values,
    const std::vector<T>& concave_values
)
```

第 2 引数が離散凹である場合の min-plus 畳み込みを返す。凹性の検査、空列の扱い、計算量は `min_plus_convolution_concave_arbitrary` と同じである。

## 実装方針

出力の添字と任意列の添字が作る階段状の有効領域を、行数・列数の大きい方で半分に分割していき、無効な要素を含まない長方形に分ける（小さい部分は全探索する）。各長方形では、凹列から得られる Monge 行列の行最小値を monotone minima で求め、長方形間で最小値をまとめる。全探索の $O(NM)$ は行わない。

候補の和と凹性判定は数学的に正確な 2 ワード表現で比較し、符号付きオーバーフローを起こさない。最終結果が `T` の表現範囲外なら `std::overflow_error`、出力長を `std::ptrdiff_t` で表せなければ `std::length_error` を送出する。
