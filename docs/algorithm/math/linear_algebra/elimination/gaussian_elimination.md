---
title: Gaussian Elimination (ガウスの消去法) [inverse_matrix] [matrix_det] [matrix_rank] [system_of_linear_equations] [system_of_linear_equations_mod_2]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/elimination/gaussian_elimination.hpp
---

体上の行列に対する Gauss の消去法と、$\mathrm{GF}(2)$ に特化した bitset 版を提供する。

通常版の要素型 `T` は、`T{}`、`T{1}`、四則演算、単項マイナス、`==` を持つ体である必要がある。典型的には `Modint` を使う。浮動小数点数に対する誤差つきのゼロ判定やピボット選択は行わない。

固定長の `Matrix<T, MAX_ROW, MAX_COL>` を受け取るオーバーロードは、最大容量がコンパイル時に決まり、作業領域を 1 つの連続した固定長配列として確保する。容量を省略した可変長の `Matrix<T>` と `std::vector<std::vector<T>>` を受け取るオーバーロードは、実際の行列サイズだけの連続した作業領域を確保する。いずれの操作でも入力行列は変更されない。

```cpp
Matrix<Mint> dynamic(rows, columns);
auto rank = gaussian_rank(dynamic);
std::vector<std::vector<Mint>> values(rows, std::vector<Mint>(columns));
auto vector_rank = gaussian_rank(values);
```

以下、行数を $R$、列数を $C$、$K=\min(R,C)$ とする。また固定長の `Matrix` 版では容量 `MAX_ROW`, `MAX_COL` をそれぞれ $R_{\max}$, $C_{\max}$ と書く。

## gaussian_rank

```cpp
int gaussian_rank(matrix)
```

行列の rank を返す。`std::vector<std::vector<T>>` 版で行の長さがそろっていなければ `std::runtime_error` を送出する。

| overload | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `vector<vector<T>>`、可変長 `Matrix<T>` | $O(RCK)$ | $O(RC)$ |
| 固定長 `Matrix<T, MAX_ROW, MAX_COL>` | $O(R_{\max}C_{\max}+RCK)$ | $O(R_{\max}C_{\max})$ |

## gaussian_determinant

```cpp
T gaussian_determinant(matrix)
```

正方行列の行列式を返す。$0\times0$ 行列の値は $1$ である。正方行列でない場合は `std::runtime_error` を送出する。

行列を $N\times N$ とする。

| overload | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `vector<vector<T>>`、可変長 `Matrix<T>` | $O(N^3)$ | $O(N^2)$ |
| 固定長 `Matrix<T, MAX_ROW, MAX_COL>` | $O(R_{\max}C_{\max}+N^3)$ | $O(R_{\max}C_{\max})$ |

## solve_linear_system

```cpp
GaussianLinearSystemSolution<T> solve_linear_system(matrix, rhs)
```

$Ax=b$ を解く。`rhs.size()` は行数と一致する必要があり、一致しなければ `std::runtime_error` を送出する。拡大係数行列の幅が `int` に収まらない場合は `std::length_error` を送出する。

戻り値は次のメンバを持つ。

- `consistent`: 解が存在するか。
- `particular`: 特解（自由変数を 0 としたもの）。解が存在しない場合は空。
- `nullspace_basis`: $Ax=0$ の解空間の基底。自由変数の列番号の昇順で並ぶ。
- `pivot_columns`: ピボット列の番号。昇順で並ぶ。

解が存在するとき、すべての解は `particular` に `nullspace_basis` の線形結合を加えた形で表せる。

出力する基底の本数を $D=C-\operatorname{rank}(A)$ とする。

| overload | 時間計算量 | 追加領域（戻り値を含む） |
| --- | --- | --- |
| `vector<vector<T>>`、可変長 `Matrix<T>` | $O(RCK+C^2)$ | $O(R(C+1)+CD)$ |
| 固定長 `Matrix<T, MAX_ROW, MAX_COL>` | $O(R_{\max}(C_{\max}+1)+RCK+C^2)$ | $O(R_{\max}(C_{\max}+1)+CD)$ |

## gaussian_inverse

```cpp
optional<Matrix<T, MAX_ROW, MAX_COL>> gaussian_inverse(matrix)
optional<vector<vector<T>>> gaussian_inverse(matrix)
```

正方行列の逆行列を返す。正則でなければ `nullopt` を返す。$0\times0$ 行列には空の行列を返す。正方行列でない場合は `std::runtime_error` を送出する。

行列を $N\times N$ とする。

| overload | 時間計算量 | 追加領域（戻り値を含む） |
| --- | --- | --- |
| `vector<vector<T>>`、可変長 `Matrix<T>` | $O(N^3)$ | $O(N^2)$ |
| 固定長 `Matrix<T, MAX_ROW, MAX_COL>` | $O(R_{\max}C_{\max}+N^3)$ | $O(R_{\max}C_{\max})$ |

## GF2LinearSystemSolution

```cpp
GF2LinearSystemSolution<MAX_COL>
```

`consistent`、`particular`、`nullspace_basis`、`pivot_columns` を持つ。通常版の `GaussianLinearSystemSolution` と同じ意味で、ベクトルは `bitset<MAX_COL>` で保存される。可変長版（`MAX_COL` を省略）ではベクトルを `std::vector<bool>` で保存する。

## gf2_rank

```cpp
int gf2_rank(vector<bitset<MAX_COL>> matrix, int columns)
int gf2_rank(const vector<vector<bool>>& matrix, int columns)
int gf2_rank(const vector<vector<bool>>& matrix)
```

$\mathrm{GF}(2)$ 上の rank を返す。bitset 版では `0 <= columns <= MAX_COL` が必要で（違反時は `std::runtime_error`）、`columns` 以上の bit は無視する。`vector<vector<bool>>` 版は内部で 64 bit 語に詰めて処理し、`columns < 0` や行の長さ不足では `std::invalid_argument` を送出する。

$W=\lceil \texttt{MAX_COL} / w\rceil$（可変長版では $\lceil C/64\rceil$）、$w$ は機械語の bit 数とする。

| 操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `gf2_rank` | $O(RC+RKW)$ | $O(RW)$ 語 |

## gf2_solve_linear_system

```cpp
GF2LinearSystemSolution<MAX_COL> gf2_solve_linear_system(
    vector<bitset<MAX_COL>> matrix,
    vector<RHS> rhs,
    int columns
)
```

$\mathrm{GF}(2)$ 上で $Ax=b$ を解く。`RHS` は `bool` に変換できればよい。`rhs.size()` は行数と一致し、`0 <= columns <= MAX_COL` である必要がある（違反時は `std::runtime_error`）。`vector<vector<bool>>` を受け取る可変長版（`columns` 省略可）もあり、形が不正なら `std::invalid_argument` を送出する。

$W=\lceil (\texttt{MAX_COL}+1) / w\rceil$、解空間の次元を $D$ とする。

| 操作 | 時間計算量 | 追加領域（戻り値を含む） |
| --- | --- | --- |
| `gf2_solve_linear_system` | $O(RC+RKW+C^2)$ | $O(RW+DW)$ 語 |

## 注意点

- 通常版の `T` は体であることを前提とする。合成数を法とする剰余環や浮動小数点数では正しく動かない。
- 形の不正（非矩形・非正方・右辺の長さ不一致・`columns` の範囲外）に対しては、通常版と bitset 版は `std::runtime_error`、`vector<vector<bool>>` 版は `std::invalid_argument` を送出する。
