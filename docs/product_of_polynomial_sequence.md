---
title: Product of Polynomial Sequence (多項式列の積) [product_of_polynomial_sequence]
documentation_of: ../src/algorithm/math/polynomial/polynomial_product.hpp
---

複数の多項式を、積木を平衡に構築してまとめて掛け合わせる。
係数は定数項から昇冪順に並べる。

## API

```cpp
#include "src/algorithm/math/polynomial/polynomial_product.hpp"

template<int MOD>
math::Polynomial<MOD> math::product_of_polynomial_sequence(
    const std::vector<math::Polynomial<MOD>>& polynomials
);
```

全要素の積を返す。空列の積は1、0多項式を含む積は0多項式である。
各段で隣り合う多項式を掛けるため、先頭から逐次掛ける場合に生じる
不必要な計算量増加を避けられる。

## API別の時間計算量・空間計算量

多項式数を `k`、次数の総和を `D`、長さ `n` の多項式積を
$M(n)=O(n\log n)$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `product_of_polynomial_sequence` | $O(M(D+1)\log(k+1))$ | $O(D+k)$ |

## 注意点

NTT に必要な2冪変換長が `MOD - 1` を割り切る必要がある。
