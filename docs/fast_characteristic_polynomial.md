---
title: Fast Characteristic Polynomial (高速特性多項式)
documentation_of: ../src/algorithm/math/linear_algebra/fast_characteristic_polynomial.hpp
---

正方行列 $A$ に対する特性多項式

$$
\det(xI-A)=\sum_{i=0}^{N}p_i x^i
$$

を Keller--Gehrig の branching algorithm で求める。
戻り値は `{p_0, p_1, ..., p_N}` の昇冪順で、`p_N` は常に `T(1)` である。

各 doubling 段では圧縮 Krylov 基底を高速行列積で延長する。
候補ベクトルを先頭から選ぶ column-rank profile も、再帰的な block elimination、
高速三角解法、Schur complement 更新で計算するため、通常の $O(N^3)$ 消去には戻らない。
最後に Krylov 基底上の表現行列から companion 対角blockを取り出し、それらの特性多項式を掛け合わせる。

## `fast_characteristic_polynomial(vector)`

```cpp
std::vector<T> coefficients = fast_characteristic_polynomial(matrix);
```

`const std::vector<std::vector<T>>&` で正方行列を受け取る。
正方形でなければ `std::invalid_argument` を送出する。
$0\times0$ 行列では `{T(1)}` を返す。

## `fast_characteristic_polynomial(Matrix)`

```cpp
std::vector<T> coefficients = fast_characteristic_polynomial(fixed_capacity_matrix);
```

`Matrix<T, MAX_ROW, MAX_COL>` 用の overload。
実際の行数と列数が異なる場合は `std::invalid_argument` を送出する。
$0\times0$ 行列にも対応する。

## API別の時間計算量・空間計算量

$N$ を行列の次数、$\omega=\log_2 7$ を使用する Strassen 行列積の指数とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| vector版 | $O(N^\omega\log N)$ | $O(N^2)$ |
| `Matrix`版 | $O(N^\omega\log N)$ | $O(N^2)$ |

## 注意点

- `T` は正確な体でなければならない。四則演算、`T{}`・`T(1)` による構築、0との等値比較を提供すること。
- 浮動小数点数は丸めにより rank 判定が変わるため対象外である。
- Strassen 積の閾値より小さいblockでは定数倍を抑える通常積・通常消去を使うが、全体の漸近計算量は変わらない。
- 除算不能な非零要素を持つ環では使用できない。
