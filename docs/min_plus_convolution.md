---
title: Min-Plus Convolution (最小加算畳み込み) [min_plus_convolution_convex_arbitrary] [min_plus_convolution_convex_convex]
documentation_of: ../src/algorithm/fft/min_plus_convolution.hpp
---

符号付き整数列 $a,b$ のmin-plus convolution

$$
c_k=\min_{i+j=k}(a_i+b_j)
$$

を、一方の列が離散凸な場合にmonotone minimaで計算する。
離散凸とは、すべての内部indexで

$$
a_{i-1}+a_{i+1}\geq 2a_i
$$

すなわち隣接差分が広義単調増加であることをいう。同じ傾きが連続してもよい。

## `is_discrete_convex`

```cpp
template<std::signed_integral T>
bool is_discrete_convex(const std::vector<T>& values)
```

`values` が離散凸かを返す。長さ0、1、2の列は常にtrue。
時間計算量は $O(N)$、追加領域は $O(1)$。

## `min_plus_convolution_convex_arbitrary`

```cpp
template<std::signed_integral T>
std::vector<T> min_plus_convolution_convex_arbitrary(
    const std::vector<T>& convex_values,
    const std::vector<T>& arbitrary_values
)
```

第1引数が離散凸な場合のmin-plus convolutionを返す。凸性は実行時に検査し、
満たさなければ `invalid_argument`。どちらかが空なら空列を返す。

## `min_plus_convolution_arbitrary_convex`

```cpp
template<std::signed_integral T>
std::vector<T> min_plus_convolution_arbitrary_convex(
    const std::vector<T>& arbitrary_values,
    const std::vector<T>& convex_values
)
```

第2引数が離散凸な場合のmin-plus convolutionを返す。凸性検査、空列の扱いは
`min_plus_convolution_convex_arbitrary` と同じ。

## `min_plus_convolution_convex_convex`

```cpp
template<std::signed_integral T>
std::vector<T> min_plus_convolution_convex_convex(
    const std::vector<T>& left_convex_values,
    const std::vector<T>& right_convex_values
)
```

両方の引数が離散凸な場合のmin-plus convolutionを返す。両方を実行時に検査し、
満たさない列があれば `invalid_argument`。結果の初項は両初項の和であり、それ以降の
隣接差分は2つの入力の隣接差分列をmergeしたものになる。この性質を用いて
$O(N+M)$ で計算する。どちらかが空なら空列。

## 計算量と実装方針

$N$ を凸列、$M$ を任意列の長さとする。出力indexを行、任意列のindexを列とした
行列

$$
A_{k,j}=\mathtt{arbitrary}_j+\mathtt{convex}_{k-j}
$$

は有効範囲が階段状のMonge行列であり、最左の行最小indexは単調増加する。
この単調性を用いたdivide-and-conquer monotone minimaで各行の最小値を求める。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `is_discrete_convex` | $O(N)$ | $O(1)$ |
| `min_plus_convolution_convex_arbitrary` | $O((N+M)\log(N+M))$ | $O(N+M)$ |
| `min_plus_convolution_arbitrary_convex` | $O((N+M)\log(N+M))$ | $O(N+M)$ |
| `min_plus_convolution_convex_convex` | $O(N+M)$ | $O(N+M)$ |

全探索の $O(NM)$ は行わない。再帰深さは $O(\log(N+M))$。

## Overflow

要素型は `std::signed_integral` に限定する。候補和、凸性判定、隣接差分は、符号と
unsignedの2word表現で数学的に正確に比較するため、C++の符号付きoverflowを起こさない。
最小値そのものが `T` の表現範囲外なら `overflow_error` を投げ、飽和やwrapは行わない。
出力長を `ptrdiff_t` で表せない場合は `length_error`。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
