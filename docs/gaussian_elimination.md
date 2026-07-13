---
title: Gaussian Elimination (ガウスの消去法)
documentation_of: ../src/algorithm/math/gaussian_elimination.hpp
---

体上の行列に対する Gaussian elimination と、$\mathrm{GF}(2)$ に特化した
bitset 版を提供する。

通常版の要素型 `T` は、`T{}`、`T{1}`、四則演算、単項マイナス、
`==` を持つ体である必要がある。典型的には `Modint` を使う。
浮動小数点数に対する誤差つきのゼロ判定や pivoting は行わない。

`Matrix<T, MAX_ROW, MAX_COL>` を受け取る overload は、最大容量がコンパイル時に
決まり、作業領域を1つの連続した固定長配列として確保する。
`vector<vector<T>>` を受け取る overload は、実際の行列サイズだけの連続作業領域を
確保する。入力行列はいずれの操作でも変更されない。

以下、行数を $R$、列数を $C$、$K=\min(R,C)$ とする。
また `Matrix` 版では $R_{max}=MAX\_ROW$、$C_{max}=MAX\_COL$ とする。

# gaussian_rank

```cpp
int gaussian_rank(matrix)
```

行列の rank を返す。

## 時間計算量

| overload | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `vector<vector<T>>` | $O(RCK)$ | $O(RC)$ |
| `Matrix<T, R_MAX, C_MAX>` | $O(R_{max}C_{max}+RCK)$ | $O(R_{max}C_{max})$ |

# gaussian_determinant

```cpp
T gaussian_determinant(matrix)
```

正方行列の determinant を返す。$0\times0$ 行列の値は $1$。
正方行列でない場合は例外を投げる。

## 時間計算量

行列を $N\times N$ とする。

| overload | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `vector<vector<T>>` | $O(N^3)$ | $O(N^2)$ |
| `Matrix<T, R_MAX, C_MAX>` | $O(R_{max}C_{max}+N^3)$ | $O(R_{max}C_{max})$ |

# solve_linear_system

```cpp
GaussianLinearSystemSolution<T> solve_linear_system(matrix, rhs)
```

$Ax=b$ を解く。`rhs.size()` は行数と一致する必要がある。

戻り値は次を持つ。

- `consistent`: 解が存在するか。
- `particular`: 特解。解が存在しない場合は空。
- `nullspace_basis`: $Ax=0$ の基底。自由変数の列番号の昇順で並ぶ。
- `pivot_columns`: pivot 列の番号。昇順で並ぶ。

解が存在するとき、すべての解は `particular` と
`nullspace_basis` の線形結合で表せる。

## 時間計算量

出力する nullspace basis の本数を $D=C-rank(A)$ とする。

| overload | 時間計算量 | 追加領域（戻り値を含む） |
| --- | --- | --- |
| `vector<vector<T>>` | $O(RCK+C^2)$ | $O(R(C+1)+CD)$ |
| `Matrix<T, R_MAX, C_MAX>` | $O(R_{max}(C_{max}+1)+RCK+C^2)$ | $O(R_{max}(C_{max}+1)+CD)$ |

# gaussian_inverse

```cpp
optional<Matrix<T, R_MAX, C_MAX>> gaussian_inverse(matrix)
optional<vector<vector<T>>> gaussian_inverse(matrix)
```

正方行列の inverse を返す。正則でなければ `nullopt`。
$0\times0$ 行列には空の行列を返す。正方行列でない場合は例外を投げる。

## 時間計算量

行列を $N\times N$ とする。

| overload | 時間計算量 | 追加領域（戻り値を含む） |
| --- | --- | --- |
| `vector<vector<T>>` | $O(N^3)$ | $O(N^2)$ |
| `Matrix<T, R_MAX, C_MAX>` | $O(R_{max}C_{max}+N^3)$ | $O(R_{max}C_{max})$ |

# GF2LinearSystemSolution

```cpp
GF2LinearSystemSolution<MAX_COL>
```

`consistent`、`particular`、`nullspace_basis`、`pivot_columns` を持つ。
通常版の `GaussianLinearSystemSolution` と同じ意味で、ベクトルは
`bitset<MAX_COL>` で保存される。

# gf2_rank

```cpp
int gf2_rank(vector<bitset<MAX_COL>> matrix, int columns)
```

$\mathrm{GF}(2)$ 上の rank を返す。`0 <= columns <= MAX_COL` が必要で、
`columns` 以上の bit は無視する。

## 時間計算量

$W=\lceil MAX\_COL / w\rceil$、$w$ は機械語の bit 数とする。

| 操作 | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `gf2_rank` | $O(RC+RKW)$ | $O(RW)$ machine words |

# gf2_solve_linear_system

```cpp
GF2LinearSystemSolution<MAX_COL> gf2_solve_linear_system(
    vector<bitset<MAX_COL>> matrix,
    vector<RHS> rhs,
    int columns
)
```

$\mathrm{GF}(2)$ 上で $Ax=b$ を解く。`RHS` は `bool` に変換できればよい。
`rhs.size()` は行数と一致し、`0 <= columns <= MAX_COL` である必要がある。

## 時間計算量

$W=\lceil (MAX\_COL+1) / w\rceil$、nullspace dimension を $D$ とする。

| 操作 | 時間計算量 | 追加領域（戻り値を含む） |
| --- | --- | --- |
| `gf2_solve_linear_system` | $O(RC+RKW+C^2)$ | $O(RW+DW)$ machine words |
