---
title: Polynomial Common Operations (多項式の基本演算)
documentation_of: ../src/algorithm/math/polynomial_common.hpp
---

NTT を使う多項式アルゴリズムの共通型と基本演算。
係数は定数項から昇冪順に並べ、末尾の0を除いた正規形で返す。
0多項式は空の `std::vector` で表す。

```cpp
#include "src/algorithm/math/polynomial_common.hpp"

using polynomial = math::Polynomial<998244353>;
```

## `Polynomial<MOD>`

```cpp
template<int MOD>
using Polynomial = std::vector<Modint<MOD>>;
```

法 `MOD` 上の多項式型。

## 多項式積

```cpp
template<int MOD>
math::Polynomial<MOD> math::polynomial_multiply(
    const math::Polynomial<MOD>& left,
    const math::Polynomial<MOD>& right
);
```

`left * right` を返す。どちらかが0多項式なら空を返す。
小さい入力では二重ループ、大きい入力では NTT に切り替える。

## 多項式和

```cpp
template<int MOD>
math::Polynomial<MOD> math::polynomial_add(
    const math::Polynomial<MOD>& left,
    const math::Polynomial<MOD>& right
);
```

`left + right` を返す。

## 形式微分

```cpp
template<int MOD>
math::Polynomial<MOD> math::polynomial_derivative(
    const math::Polynomial<MOD>& polynomial
);
```

多項式の形式微分を返す。定数多項式と0多項式には空を返す。

## 計算量

`n = left.size()`, `m = right.size()` とし、長さ `k` の多項式積の計算量を
`M(k)` と書く。この実装では NTT を使う範囲で
$M(k)=O(k\log k)$ である。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `polynomial_multiply` | $O(M(n+m))$ | $O(n+m)$ |
| `polynomial_add` | $O(\max(n,m))$ | $O(\max(n,m))$ |
| `polynomial_derivative` | $O(n)$ | $O(n)$ |

## 前提

- NTT を使う大きさでは、必要な2冪変換長が `MOD - 1` を割り切る必要がある。
- 除算を伴う上位アルゴリズムでは `Modint<MOD>` の必要な非零要素が逆元を持つ必要がある。
