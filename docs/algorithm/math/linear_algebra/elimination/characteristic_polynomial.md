---
title: Characteristic Polynomial (特性多項式) [characteristic_polynomial]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/elimination/characteristic_polynomial.hpp
---

正方行列 $A$ の特性多項式

$$
\det(xI-A)=\sum_{i=0}^{N}p_i x^i
$$

を求める。相似変換で上 Hessenberg 行列にした後、左上の主小行列の特性多項式を順に計算する。戻り値は `{p_0, p_1, ..., p_N}` の順で、末尾は常に 1 である。

要素型 `T` は体で、四則演算、0 との比較、`T(1)` を提供する必要がある。

## 使い方

```cpp
using Mint = Modint998244353;
std::vector<std::vector<Mint>> values{ {1, 2}, {3, 4} };
auto from_vector = characteristic_polynomial(values);
Matrix<Mint> matrix(values);
auto from_matrix = characteristic_polynomial(matrix);
```

## `characteristic_polynomial(vector)`

```cpp
std::vector<T> coefficients = characteristic_polynomial(matrix);
```

`std::vector<std::vector<T>>` を値渡しで受け取る。正方行列でなければ `std::invalid_argument` を送出する。$0\times0$ 行列に対しては `{T(1)}` を返す。

## `characteristic_polynomial(Matrix)`

```cpp
std::vector<T> coefficients = characteristic_polynomial(matrix);
```

`Matrix<T, MAX_ROW, MAX_COL>` 用のオーバーロード。容量を省略した可変長の `Matrix<T>` も、容量を明示した固定長の `Matrix<T, R, C>` も渡せる。実際の行数と列数が異なる場合は `std::invalid_argument` を送出する。入力を `std::vector` の行列にコピーしてから同じ計算を行う。

## 時間計算量・空間計算量

$N$ を行列の次数とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `vector` 版 | $O(N^3)$ | $O(N^2)$ |
| `Matrix` 版 | $O(N^3)$ | $O(N^2)$ |

## 注意点

- `T` は体であることを前提とする（Hessenberg 化でピボットの逆元を使う）。合成数を法とする剰余環では正しく動かない。
