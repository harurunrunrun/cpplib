---
title: Fast Characteristic Polynomial (高速特性多項式)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/elimination/fast_characteristic_polynomial.hpp
---

正方行列 $A$ の特性多項式

$$
\det(xI-A)=\sum_{i=0}^{N}p_i x^i
$$

を Keller-Gehrig の branching algorithm で求める。戻り値は `{p_0, p_1, ..., p_N}` の昇冪順で、`p_N` は常に `T(1)` である。

倍々に進む各段では、圧縮した Krylov 基底を高速行列積で延長する。候補ベクトルを先頭から選ぶ列の rank profile も、再帰的なブロック消去、高速な三角行列の求解、Schur 補行列の更新で計算するため、通常の $O(N^3)$ の消去には戻らない。最後に Krylov 基底上の表現行列からコンパニオン行列の対角ブロックを取り出し、それらの特性多項式を掛け合わせる。

## 使い方

```cpp
using Mint = Modint998244353;
std::vector<std::vector<Mint>> values{ {1, 2}, {3, 4} };
auto from_vector = fast_characteristic_polynomial(values);
Matrix<Mint> matrix(values);
auto from_matrix = fast_characteristic_polynomial(matrix);
```

## `fast_characteristic_polynomial(vector)`

```cpp
std::vector<T> coefficients = fast_characteristic_polynomial(matrix);
```

`const std::vector<std::vector<T>>&` で正方行列を受け取る。正方行列でなければ `std::invalid_argument` を送出する。$0\times0$ 行列では `{T(1)}` を返す。

## `fast_characteristic_polynomial(Matrix)`

```cpp
std::vector<T> coefficients = fast_characteristic_polynomial(matrix);
```

`Matrix<T, MAX_ROW, MAX_COL>` 用のオーバーロード。容量を省略した可変長の `Matrix<T>` も、容量を明示した固定長の `Matrix<T, R, C>` も渡せる。実際の行数と列数が異なる場合は `std::invalid_argument` を送出する。$0\times0$ 行列にも対応する。

## 時間計算量・空間計算量

$N$ を行列の次数、$\omega=\log_2 7$ を使用する Strassen 行列積の指数とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `vector` 版 | $O(N^\omega\log N)$ | $O(N^2)$ |
| `Matrix` 版 | $O(N^\omega\log N)$ | $O(N^2)$ |

## 注意点

- `T` は正確な体でなければならない。四則演算、`T{}`・`T(1)` による構築、0 との等値比較を提供する必要がある。0 でない要素で割れない環では使えない。
- 浮動小数点数は丸めにより rank の判定が変わるため対象外である。
- $N \le 32$ のブロックでは定数倍を抑えるため通常の行列積・消去を使うが、全体の漸近計算量は変わらない。
