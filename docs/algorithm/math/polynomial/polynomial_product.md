---
title: Product of Polynomial Sequence (多項式列の積) [product_of_polynomial_sequence]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_product.hpp
---

複数の多項式を、隣り合うもの同士を掛け合わせる平衡な積木を構築してまとめて掛け合わせる。
係数は定数項から昇冪順に並べる。

## API

```cpp
#include "src/algorithm/math/polynomial/polynomial_product.hpp"

template<int MOD>
math::Polynomial<MOD> math::product_of_polynomial_sequence(
    const std::vector<math::Polynomial<MOD>>& polynomials
);
```

全要素の積を返す。空列の積は $1$、零多項式を含む場合の積は零多項式（空の列）である。
各段で隣り合う多項式を掛けるため、先頭から逐次掛ける場合に生じる不必要な計算量の増加を避けられる。

## API別の時間計算量・空間計算量

多項式の個数を $k$、次数の総和を $D$、長さ $n$ の多項式乗算の時間を $M(n) = O(n \log n)$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `product_of_polynomial_sequence` | $O(M(D+1) \log (k+1))$ | $O(D+k)$ |

## 注意点

乗算は NTT で行うため、必要な 2 の冪の変換長が `MOD - 1` を割り切る必要がある（割り切らない場合は `std::runtime_error`）。
