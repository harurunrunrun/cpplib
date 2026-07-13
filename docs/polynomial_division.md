---
title: Division of Polynomials
documentation_of: ../src/algorithm/math/polynomial_division.hpp
---

体上の多項式除算を、反転した多項式の形式的冪級数逆元を使って高速に行う。
係数は定数項から昇冪順に並べる。0多項式は空の `std::vector` で表す。

```cpp
#include "src/algorithm/math/polynomial_division.hpp"

using polynomial = math::Polynomial<998244353>;
```

## 商と余り

```cpp
template<int MOD>
math::PolynomialDivModResult<MOD> math::polynomial_divmod(
    math::Polynomial<MOD> dividend,
    math::Polynomial<MOD> divisor
);
```

次を満たす `quotient` と `remainder` を返す。

$$
dividend = quotient \cdot divisor + remainder,
\qquad \deg(remainder) < \deg(divisor)
$$

`PolynomialDivModResult<MOD>` は次の2メンバを持つ。

- `quotient`: 商
- `remainder`: 余り

入力末尾の0は無視され、結果は正規化される。0多項式による除算は
`std::invalid_argument` を送出する。

## 余りだけを求める

```cpp
template<int MOD>
math::Polynomial<MOD> math::polynomial_remainder(
    math::Polynomial<MOD> dividend,
    math::Polynomial<MOD> divisor
);
```

`polynomial_divmod` の余りだけを返す。例外条件は同じである。

## 計算量

`n = dividend.size()`, `m = divisor.size()` とし、NTT による長さ `k` の
多項式積を $M(k)=O(k\log k)$ とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `polynomial_divmod` | $O(M(n))$ | $O(n)$ |
| `polynomial_remainder` | $O(M(n))$ | $O(n)$ |

小さい入力では定数を抑えるため $O(nm)$ の筆算へ切り替える。

## 前提

- `divisor` の最高次係数が逆元を持つ必要がある。
- NTT に必要な2冪変換長が `MOD - 1` を割り切る必要がある。
