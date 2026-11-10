---
title: Fast Gaussian Elimination (高速ガウス消去法)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/elimination/fast_gaussian_elimination.hpp
---

## 概要

体上の矩形行列について、rank、連立一次方程式の解空間、逆行列、行列式を高速行列積を用いて求める。`fast_characteristic_polynomial.hpp` の ordered rank profile と Strassen 行列積を再利用し、通常の Gauss の消去法の $O(S^3)$ を $O(S^\omega)$ に改善する。ここで $S=\max(R,C)$, $\omega=\log_2 7$ である。

連立一次方程式では元の列順に対する列の rank profile を選ぶ。そのため、返すピボット列は `solve_linear_system` と同じく元の添字の昇順になる。拡大係数行列の rank で解の存在を判定し、上三角のピボットブロックに対する複数右辺の再帰的な後退代入により、特解と解空間の基底を一括で構成する。

```cpp
using Mint = Modint998244353;
std::vector<std::vector<Mint>> values{ {1, 2}, {3, 4} };
Matrix<Mint> matrix(values);
auto rank = fast_gaussian_rank(matrix);
auto inverse = fast_gaussian_inverse(matrix);
auto solution = fast_solve_linear_system(matrix, std::vector<Mint>{5, 11});
```

## Rank

```cpp
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
```

行列の rank を返す。一次元配列版は行優先（row-major）で、要素数が `rows * columns` と一致しなければならない。二次元配列版は全行が同じ長さでなければならない。`Matrix` 版（固定長・可変長とも）は容量ではなく実際のサイズを使う。0 行または 0 列の行列の rank は 0 である。

## 連立一次方程式

```cpp
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
```

`A x = right_hand_side` の解空間を返す。返り値は通常版と共通の `GaussianLinearSystemSolution<T>` で、各メンバは次を表す。

- `consistent`: 解が存在するか。
- `particular`: 自由変数を 0 とした特解。解が存在しなければ空である。
- `nullspace_basis`: `A x = 0` の解空間の基底。対応する自由列の昇順で並ぶ。
- `pivot_columns`: 元の行列におけるピボット列。昇順で並ぶ。

解が存在する場合、任意の係数 `c[i]` に対して `particular + sum(c[i] * nullspace_basis[i])` が全解を表す。0 行 $C$ 列の `Matrix` では長さ $C$ の零ベクトルと標準基底を返す。係数行列の行数と右辺の長さは一致しなければならない。

## 逆行列

```cpp
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
```

正方行列の逆行列を返す。特異なら `nullopt` を返す。一次元配列版の返り値は行優先である。0 次の正方行列の逆行列は空行列として存在する。

## 行列式

```cpp
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
```

`fast_determinant` と同じ高速行列式を、Gauss の消去法の API 群から使うための別名である。正方行列の行列式を返し、特異なら 0、0 次なら 1 を返す。

## 時間計算量

$R$ を行数、$C$ を列数、$S=\max(R,C)$、$F=C-\operatorname{rank}$、$\omega=\log_2 7$ とし、`T` の各演算を $O(1)$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `fast_gaussian_rank` | $O(S^\omega)$ | $O(S^2)$ |
| `fast_solve_linear_system` | $O(S^\omega + CF)$ | $O(S^2 + CF)$ |
| `fast_gaussian_inverse` | $O(S^\omega)$ | $O(S^2)$ |
| `fast_gaussian_determinant` | $O(S^\omega)$ | $O(S^2)$ |

$CF$ は解空間の基底そのものの出力量であり、省略できない。小さいブロックでは定数倍を抑えるため通常の後退代入に切り替えるが、漸近計算量は変わらない。

## 注意点

- `T{}` が 0、`T(1)` が 1 を表し、加減乗除、単項マイナス、等値比較ができる正確な体を要求する。
- 浮動小数点数では丸め誤差により 0 判定と rank 判定が不安定になるため、この API の前提を満たさない。
- 一次元配列の要素数の不一致、二次元配列の非矩形、右辺の長さの不一致、逆行列・行列式に対する非正方の入力では `invalid_argument` を送出する。
- 要素数の積または解の列添字が表現範囲を超える場合は `length_error` を送出する。
- 計算中に選んだピボットが 0 になるなど、`T` が体の前提を破った場合は `logic_error` を送出する。
