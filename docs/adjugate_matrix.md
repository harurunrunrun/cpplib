---
title: Adjugate Matrix
documentation_of: ../src/algorithm/math/adjugate_matrix.hpp
---

正方行列 $A$ の余因子行列

$$
\operatorname{adj}(A)_{i,j}=(-1)^{i+j}\det A_{\widehat{j},\widehat{i}}
$$

を求める。$A$ が正則でない場合も扱う。

## `adjugate_matrix(vector)`

```cpp
std::vector<std::vector<T>> result = adjugate_matrix(matrix);
```

`std::vector<std::vector<T>>` で与えた正方行列の余因子行列を返す。正方行列でない場合は `std::invalid_argument` を送出する。$0\times0$ 行列に対しては空行列を返す。

要素型 `T` は体であり、零元・単位元、四則演算、単項マイナス、等値比較を備える必要がある。階数が $N$ ならば $\det(A)A^{-1}$ を、階数が $N-1$ ならば左右の零空間ベクトルと非零な余因子を、階数が $N-2$ 以下ならば零行列を返す。

## `adjugate_matrix(Matrix)`

```cpp
auto result = adjugate_matrix(fixed_capacity_matrix);
```

`Matrix<T, MAX_ROW, MAX_COL>` 用のオーバーロード。返り値も同じ型の `Matrix` である。実際の行数と列数が一致しない場合は `std::invalid_argument` を送出する。

## 計算量

$N$ を行列の次数とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `adjugate_matrix(vector)` | $O(N^3)$ | $O(N^2)$ |
| `adjugate_matrix(Matrix)` | $O(N^3)$ | $O(N^2)$ |

Gauss--Jordan 消去と同時に行基本変形を記録する。
