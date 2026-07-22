---
title: Fast Gaussian Elimination (高速ガウス消去法)
documentation_of: ../src/algorithm/math/linear_algebra/fast_gaussian_elimination.hpp
---

## 概要

体上の矩形行列について、rank、連立一次方程式の解空間、逆行列、行列式を高速行列積を用いて求める。`fast_characteristic_polynomial.hpp` のordered rank profileとStrassen行列積を再利用し、通常のGaussian eliminationの `O(S^3)` を `O(S^omega)` に改善する。ここで `S=max(R,C)`, `omega=log_2(7)` である。

連立一次方程式では元の列順に対する列ランクプロファイルを選ぶ。そのため、返すpivot列は `solve_linear_system` と同じく元の添字で昇順となる。拡大係数行列のrankで整合性を判定し、上三角pivot blockに対する複数右辺の再帰的後退代入により、特解とnullspace basisを一括で構成する。

## Rank

~~~cpp
template<class T>
std::size_t fast_gaussian_rank(
    const std::vector<T>& matrix,
    std::size_t rows,
    std::size_t columns
);

template<class T>
std::size_t fast_gaussian_rank(
    const std::vector<std::vector<T>>& matrix
);

template<class T, int MAX_ROW, int MAX_COL>
std::size_t fast_gaussian_rank(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix
);
~~~

行列のrankを返す。一次元配列版はrow-majorであり、要素数が `rows * columns` と一致しなければならない。二次元配列版は全行が同じ長さでなければならない。`Matrix` 版は容量ではなく実寸を使う。0行または0列のrankは0である。

## 連立一次方程式

~~~cpp
template<class T>
GaussianLinearSystemSolution<T> fast_solve_linear_system(
    const std::vector<T>& matrix,
    std::size_t rows,
    std::size_t columns,
    const std::vector<T>& right_hand_side
);

template<class T>
GaussianLinearSystemSolution<T> fast_solve_linear_system(
    const std::vector<std::vector<T>>& matrix,
    const std::vector<T>& right_hand_side
);

template<class T, int MAX_ROW, int MAX_COL>
GaussianLinearSystemSolution<T> fast_solve_linear_system(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix,
    const std::vector<T>& right_hand_side
);
~~~

`A x = right_hand_side` の解空間を返す。返り値は通常版と共通の `GaussianLinearSystemSolution<T>` で、各memberは次を表す。

- `consistent`: 解が存在するか。
- `particular`: 自由変数を0とした特解。非整合なら空である。
- `nullspace_basis`: `A x = 0` のbasis。対応する自由列の昇順で並ぶ。
- `pivot_columns`: 元の行列におけるpivot列。昇順で並ぶ。

解が存在する場合、任意の係数 `c[i]` に対して `particular + sum(c[i] * nullspace_basis[i])` が全解を表す。0行 `C` 列の `Matrix` では長さ `C` の零ベクトルと標準basisを返す。係数行列の行数と右辺の長さは一致しなければならない。

## 逆行列

~~~cpp
template<class T>
std::optional<std::vector<T>> fast_gaussian_inverse(
    const std::vector<T>& matrix,
    std::size_t size
);

template<class T>
std::optional<std::vector<std::vector<T>>> fast_gaussian_inverse(
    const std::vector<std::vector<T>>& matrix
);

template<class T, int MAX_ROW, int MAX_COL>
std::optional<Matrix<T, MAX_ROW, MAX_COL>> fast_gaussian_inverse(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix
);
~~~

正方行列の逆行列を返す。特異なら `nullopt` を返す。一次元配列版の返り値はrow-majorである。0次正方行列の逆行列は空行列として存在する。

## 行列式

~~~cpp
template<class T>
T fast_gaussian_determinant(
    const std::vector<T>& matrix,
    std::size_t size
);

template<class T>
T fast_gaussian_determinant(
    const std::vector<std::vector<T>>& matrix
);

template<class T, int MAX_ROW, int MAX_COL>
T fast_gaussian_determinant(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix
);
~~~

`fast_determinant` と同じ高速行列式APIをGaussian elimination群から使うためのaliasである。正方行列の行列式を返し、特異なら0、0次なら1を返す。

## 時間計算量

`R=rows`, `C=columns`, `S=max(R,C)`, `F=C-rank`, `omega=log_2(7)` とする。`T` の各演算は `O(1)` とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `fast_gaussian_rank` | `O(S^omega)` | `O(S^2)` |
| `fast_solve_linear_system` | `O(S^omega + C F)` | `O(S^2 + C F)` |
| `fast_gaussian_inverse` | `O(S^omega)` | `O(S^2)` |
| `fast_gaussian_determinant` | `O(S^omega)` | `O(S^2)` |

`C F` はnullspace basisそのものの出力量であり、省略できない。小さいblockでは定数を抑えるため通常の後退代入へ切り替えるが、漸近計算量は変わらない。

## 注意点

- `T{}` が0、`T(1)` が1を表し、加減乗除、単項minus、等値比較ができる正確な体を要求する。
- 浮動小数点数では丸め誤差により0判定とrank判定が不安定になるため、このAPIの契約を満たさない。
- 一次元配列の要素数不一致、二次元配列の非矩形、右辺の長さ不一致、逆行列・行列式に対する非正方形入力では `invalid_argument`。
- 要素数の積または解の列添字が表現範囲を超える場合は `length_error`。
- 計算中に選ばれたpivotが0になるなど、`T` が体の契約を破った場合は `logic_error`。
