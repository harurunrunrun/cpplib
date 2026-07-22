---
title: Fast Adjugate Matrix (高速余因子行列)
documentation_of: ../src/algorithm/math/linear_algebra/fast_adjugate_matrix.hpp
---

## 概要

体上の正方行列 $A$ に対して余因子行列

\[
\operatorname{adj}(A)_{i,j}=(-1)^{i+j}
\det A_{\widehat{j},\widehat{i}}
\]

を求める。正則行列だけでなく、特異行列にも使える。

## 一次元配列

~~~cpp
template<class T>
std::vector<T> fast_adjugate_matrix(
    const std::vector<T>& matrix,
    std::size_t size
);
~~~

row-major な `size x size` 行列の余因子行列を row-major で返す。
`matrix.size() == size * size` を要求する。`size == 0` では空配列を返す。

## 二次元配列

~~~cpp
template<class T>
std::vector<std::vector<T>> fast_adjugate_matrix(
    const std::vector<std::vector<T>>& matrix
);
~~~

二次元配列で与えた正方行列の余因子行列を返す。0行の入力も許可する。

## 固定容量行列

~~~cpp
template<class T, int MAX_ROW, int MAX_COL>
Matrix<T, MAX_ROW, MAX_COL> fast_adjugate_matrix(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix
);
~~~

`Matrix` の実寸 `rows() x cols()` が正方形のとき、同じ型の余因子行列を返す。

## アルゴリズム

ordered row-echelon form で階数を求め、階数ごとに処理を分ける。

- $\operatorname{rank}(A)=N$ なら、$\det(A)A^{-1}$ を返す。
- $\operatorname{rank}(A)\leq N-2$ なら、すべての $(N-1)$ 次小行列が0なので零行列を返す。
- $\operatorname{rank}(A)=N-1$ なら、$A$ と $A^\mathsf{T}$ の
  ordered row-echelon form から右零空間ベクトル $v$ と左零空間ベクトル $u$ を復元する。
  このとき $\operatorname{adj}(A)=cvu^\mathsf{T}$ である。$v_i\neq0$, $u_j\neq0$
  となる成分を選び、row $j$ と column $i$ を削除した小行列の高速行列式
  $(-1)^{i+j}\det A_{\widehat j,\widehat i}$ から $c$ を決定する。

$N=1$ の零行列に対しても、0次小行列の行列式を1として `[1]` を返す。

## 時間計算量

$N$ を行列の次数、$\omega=\log_2 7$ とし、`T` の各演算を $O(1)$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 一次元配列版 | $O(N^\omega)$ | $O(N^2)$ |
| 二次元配列版 | $O(N^\omega)$ | $O(N^2)$ |
| `Matrix` 版 | $O(N^\omega)$ | $O(N^2)$ |

rank profile、逆行列、必要な1個の minor determinant は Strassen 行列積を用いる。
零空間の復元と外積は $O(N^2)$ である。

## 注意点

- `T{}` が0、`T(1)` が1を表し、加減乗除、単項マイナス、等値比較ができる正確な体を要求する。
- 浮動小数点数では厳密な0判定を契約できないため使用しない。
- 一次元配列の要素数不一致、二次元配列または `Matrix` の非正方形入力では `std::invalid_argument` を送出する。
- `size * size` が `size_t` に収まらない場合は `std::length_error` を送出する。
