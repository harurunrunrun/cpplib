---
title: Fast Determinant (高速行列式)
documentation_of: ../src/algorithm/math/linear_algebra/fast_determinant.hpp
---

## 概要

体上の正方行列の行列式を、rank profile、block Schur complement、Strassen行列積で求める。先頭blockが正則になるよう列を決定的に並べ替えるため、先頭主小行列が特異な行列にも使える。列置換の符号は結果へ反映され、入力全体が特異なら `T()` を返す。

## 一次元配列

~~~cpp
template<class T>
T fast_determinant(
    const std::vector<T>& matrix,
    std::size_t size
);
~~~

row-majorな `size x size` 行列の行列式を返す。`matrix.size() == size * size` を要求する。`size == 0` では空行列の行列式 `T(1)` を返す。

## 二次元配列

~~~cpp
template<class T>
T fast_determinant(const std::vector<std::vector<T>>& matrix);
~~~

各行の長さが `matrix.size()` である正方行列の行列式を返す。0行の入力も許可する。

## 固定容量行列

~~~cpp
template<class T, int MAX_ROW, int MAX_COL>
T fast_determinant(const Matrix<T, MAX_ROW, MAX_COL>& matrix);
~~~

実寸が正方形である `Matrix` の行列式を返す。容量 `MAX_ROW`, `MAX_COL` ではなく、`rows()` と `cols()` で表される実寸だけを参照する。

## アルゴリズム

行列を上段 `k=floor(N/2)` 行と下段に分ける。上段のordered rank profileから列置換 `P` を選び、

\[
AP=\begin{pmatrix}B&C\\D&E\end{pmatrix}
\]

の `B` を正則にする。元の行列が正則なら任意の行部分集合も独立なので、この `B` は必ず存在する。次を再帰的に計算する。

\[
\det(A)=\operatorname{sgn}(P)\det(B)
\det(E-DB^{-1}C).
\]

上段のrankが `k` 未満なら、入力の行が一次従属なので直ちに0を返す。Schur complementの行列積とblock逆行列にはStrassen法を使う。列置換は `new column -> old column` の置換としてcycle分解し、`(-1)^(N-cycles)` を掛ける。

## 時間計算量

`N=size`、`omega=log_2(7)` とし、`T` の各演算を `O(1)` とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 一次元配列版 | `O(N^omega)` | `O(N^2)` |
| 二次元配列版 | `O(N^omega)` | `O(N^2)` |
| `Matrix` 版 | `O(N^omega)` | `O(N^2)` |

`N <= 32` では定数を小さくするためGaussian eliminationへ切り替える。この閾値は漸近計算量を変えない。

## 注意点

- `T{}` が0、`T(1)` が1を表し、加減乗除、単項minus、等値比較ができる正確な体を要求する。
- 浮動小数点数は丸め誤差のため、0判定とrank判定の契約を満たさない。実数近似の行列式には使わない。
- 一次元配列の要素数不一致、二次元配列または `Matrix` の非正方形入力では `invalid_argument`。
- `size * size` が `size_t` に収まらない場合は `length_error`。
