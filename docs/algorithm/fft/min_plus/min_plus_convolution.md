---
title: Min-Plus Convolution (最小加算畳み込み) [min_plus_convolution_convex_arbitrary] [min_plus_convolution_convex_convex]
documentation_of: ../../../../src/algorithm/fft/min_plus/min_plus_convolution.hpp
---

符号付き整数列 $a, b$ の min-plus 畳み込み

$$
c_k=\min_{i+j=k}(a_i+b_j)
$$

を、一方の列が離散凸である場合に monotone minima で計算する。離散凸とは、すべての内部の添字 $i$ で

$$
a_{i-1}+a_{i+1}\geq 2a_i
$$

が成り立つこと、すなわち隣接差分が広義単調増加であることをいう。同じ傾きが連続してもよい。

## `is_discrete_convex`

```cpp
template<std::signed_integral T>
bool is_discrete_convex(const std::vector<T>& values)
```

`values` が離散凸かどうかを返す。長さ $0, 1, 2$ の列に対しては常に `true` を返す。

## `min_plus_convolution_convex_arbitrary`

```cpp
template<std::signed_integral T>
std::vector<T> min_plus_convolution_convex_arbitrary(
    const std::vector<T>& convex_values,
    const std::vector<T>& arbitrary_values
)
```

第 1 引数が離散凸である場合の min-plus 畳み込みを返す。凸性は実行時に検査し、満たさなければ `std::invalid_argument` を送出する。どちらかが空なら空列を返す。

## `min_plus_convolution_arbitrary_convex`

```cpp
template<std::signed_integral T>
std::vector<T> min_plus_convolution_arbitrary_convex(
    const std::vector<T>& arbitrary_values,
    const std::vector<T>& convex_values
)
```

第 2 引数が離散凸である場合の min-plus 畳み込みを返す。凸性の検査と空列の扱いは `min_plus_convolution_convex_arbitrary` と同じである。

## `min_plus_convolution_convex_convex`

```cpp
template<std::signed_integral T>
std::vector<T> min_plus_convolution_convex_convex(
    const std::vector<T>& left_convex_values,
    const std::vector<T>& right_convex_values
)
```

両方の引数が離散凸である場合の min-plus 畳み込みを返す。両方を実行時に検査し、満たさない列があれば `std::invalid_argument` を送出する。結果の初項は両初項の和であり、それ以降の隣接差分は 2 つの入力の隣接差分列をマージしたものになる。この性質を用いて $O(N + M)$ で計算する。どちらかが空なら空列を返す。

## 計算量と実装方針

$N$ を凸列、$M$ を任意列の長さとする。出力の添字を行、任意列の添字を列とした行列

$$
A_{k,j}=\mathtt{arbitrary}_j+\mathtt{convex}_{k-j}
$$

は有効範囲が階段状の Monge 行列であり、各行で最小値をとる最左の列は行について単調増加する。この単調性を用いた分割統治の monotone minima で各行の最小値を求める。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `is_discrete_convex` | $O(N)$ | $O(1)$ |
| `min_plus_convolution_convex_arbitrary` | $O((N+M)\log(N+M))$ | $O(N+M)$ |
| `min_plus_convolution_arbitrary_convex` | $O((N+M)\log(N+M))$ | $O(N+M)$ |
| `min_plus_convolution_convex_convex` | $O(N+M)$ | $O(N+M)$ |

全探索の $O(NM)$ は行わない。再帰の深さは $O(\log(N+M))$ である。

## オーバーフロー

要素型は `std::signed_integral` に限る。候補の和、凸性判定、隣接差分は、符号と符号なし 2 ワードの絶対値による表現で数学的に正確に比較するため、C++ の符号付きオーバーフローを起こさない。最小値そのものが `T` の表現範囲外なら `std::overflow_error` を送出し、飽和やラップアラウンドは行わない。出力長を `std::ptrdiff_t` で表せない場合は `std::length_error` を送出する。
