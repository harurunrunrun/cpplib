---
title: Min-Plus Convolution (Concave and Arbitrary) (凹列と任意列の最小加算畳み込み)
documentation_of: ../src/algorithm/fft/min_plus_convolution_concave.hpp
---

符号付き整数列の一方が離散凹であるとき、min-plus convolution

$$
c_k=\min_{i+j=k}(a_i+b_j)
$$

を計算する。離散凹とは、すべての内部indexで

$$
a_{i-1}+a_{i+1}\leq 2a_i
$$

すなわち隣接差分が広義単調減少することをいう。

## `is_discrete_concave`

```cpp
template<std::signed_integral T>
bool is_discrete_concave(const std::vector<T>& values)
```

`values` が離散凹ならtrueを返す。長さ0、1、2の列は常にtrue。

- 時間計算量: $O(N)$
- 空間計算量（追加領域）: $O(1)$

## `min_plus_convolution_concave_arbitrary`

```cpp
template<std::signed_integral T>
std::vector<T> min_plus_convolution_concave_arbitrary(
    const std::vector<T>& concave_values,
    const std::vector<T>& arbitrary_values
)
```

第1引数が離散凹な場合のmin-plus convolutionを返す。凹性は実行時に検査し、
満たさなければ `invalid_argument` を投げる。どちらかが空なら空列を返す。

- 時間計算量: $O((N+M)\log(N+M))$
- 空間計算量（追加領域）: $O(N+M)$

## `min_plus_convolution_arbitrary_concave`

```cpp
template<std::signed_integral T>
std::vector<T> min_plus_convolution_arbitrary_concave(
    const std::vector<T>& arbitrary_values,
    const std::vector<T>& concave_values
)
```

第2引数が離散凹な場合のmin-plus convolutionを返す。凹性検査、空列の扱い、
計算量は `min_plus_convolution_concave_arbitrary` と同じ。

## 実装方針

出力indexと任意列のindexが作る階段状の有効領域を、無効要素を含まない矩形へ
分割する。各矩形では凹列によって得られるMonge行列の行最小値をmonotone minimaで
求め、矩形間の最小値を統合する。全探索の $O(NM)$ は行わない。

候補和と凹性判定は数学的に正確な2word表現で比較し、符号付きoverflowを起こさない。
最終結果が `T` の表現範囲外なら `overflow_error`、出力長を `ptrdiff_t` で表せなければ
`length_error` を投げる。

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
