---
title: Characteristic Polynomial (特性多項式) [characteristic_polynomial]
documentation_of: ../../../../../src/algorithm/math/linear_algebra/elimination/characteristic_polynomial.hpp
---

正方行列$A$の特性多項式

$$
\det(xI-A)=\sum_{i=0}^{N}p_i x^i
$$

を求める。相似変換で上Hessenberg行列にした後、左上主小行列の特性多項式を順に計算する。
戻り値は`{p_0, p_1, ..., p_N}`の順であり、末尾は常に1である。

要素型`T`は体であり、四則演算、0との比較、`T(1)`を提供する必要がある。

## `characteristic_polynomial(vector)`

```cpp
std::vector<T> coefficients = characteristic_polynomial(matrix);
```

`std::vector<std::vector<T>>`を値渡しで受け取る。
正方形でなければ`std::invalid_argument`を送出する。
$0\times0$行列に対しては`{T(1)}`を返す。

## `characteristic_polynomial(Matrix)`

```cpp
std::vector<T> coefficients = characteristic_polynomial(fixed_capacity_matrix);
```

`Matrix<T, MAX_ROW, MAX_COL>`用のoverload。
実際の行数と列数が異なる場合は`std::invalid_argument`を送出する。
入力を動的行列へコピーしてから同じ計算を行う。

## API別の時間計算量・空間計算量

$N$を行列の次数とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| vector版 | $O(N^3)$ | $O(N^2)$ |
| `Matrix`版 | $O(N^3)$ | $O(N^2)$ |

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。

## 容量を省略する可変長版

~~~cpp
using Mint = Modint998244353;
std::vector<std::vector<Mint>> values{ {1, 2}, {3, 4} };
auto from_vector = characteristic_polynomial(values);
Matrix<Mint> matrix(values);
auto from_matrix = characteristic_polynomial(matrix);
~~~

`vector` overload では容量のテンプレート引数を使わない。
`Matrix<Mint>` も実寸だけの `vector` 保存領域を使い、同じ関数へ渡せる。
実行時の次数を $N$、高速行列積の指数を $\omega=\log_2 7$ とすると、
時間計算量は $O(N^3)$、追加空間計算量は $O(N^2)$。
固定容量を明示する `Matrix<Mint, R, C>` も同じ header の overload で利用できる。
