---
title: Polynomial Common Operations (多項式の基本演算)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_common.hpp
---

NTT を使う多項式アルゴリズムの共通の型と基本演算。係数は定数項から次数の昇順に並べ、戻り値は末尾の $0$ を取り除いた正規形にする。零多項式は空の `std::vector` で表す。

```cpp
#include "src/algorithm/math/polynomial/polynomial_common.hpp"

using polynomial = math::Polynomial<998244353>;
```

## `Polynomial<MOD>`

```cpp
template<int MOD>
using Polynomial = std::vector<Modint<MOD>>;
```

`MOD` を法とする係数を持つ多項式の型。

## 多項式積

```cpp
template<int MOD>
math::Polynomial<MOD> math::polynomial_multiply(
    const math::Polynomial<MOD>& left,
    const math::Polynomial<MOD>& right
);
```

`left * right` を返す。どちらかが零多項式なら空の列を返す。長さの積が $4096$ 以下なら二重ループ、それより大きければ NTT で計算する。

## 多項式和

```cpp
template<int MOD>
math::Polynomial<MOD> math::polynomial_add(
    const math::Polynomial<MOD>& left,
    const math::Polynomial<MOD>& right
);
```

`left + right` を返す（末尾の $0$ は取り除く）。

## 形式微分

```cpp
template<int MOD>
math::Polynomial<MOD> math::polynomial_derivative(
    const math::Polynomial<MOD>& polynomial
);
```

多項式の形式微分を返す。定数多項式と零多項式には空の列を返す。

## 時間計算量・空間計算量

$n$ を `left.size()`、$m$ を `right.size()` とし、長さ $k$ の多項式の積の計算量を $M(k)$ と書く。この実装では NTT を使う範囲で $M(k)=O(k\log k)$ である。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `polynomial_multiply` | $O(M(n+m))$ | $O(n+m)$ |
| `polynomial_add` | $O(\max(n,m))$ | $O(\max(n,m))$ |
| `polynomial_derivative` | $O(n)$ | $O(n)$ |

## 注意点

- NTT を使う大きさでは、必要な 2 の冪の変換長が $\mathrm{MOD} - 1$ を割り切る必要がある。
- 除算を伴う上位のアルゴリズムでは、`Modint<MOD>` の必要な非零の要素が逆元を持つ必要がある。
