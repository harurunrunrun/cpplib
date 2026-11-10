---
title: Division of Polynomials (多項式除算) [division_of_polynomials]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_division.hpp
---

体上の多項式の除算を、係数を反転した多項式の形式的冪級数としての逆元を使って高速に行う。係数は定数項から次数の昇順に並べ、零多項式は空の `std::vector` で表す。

```cpp
#include "src/algorithm/math/polynomial/polynomial_division.hpp"

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
\mathrm{dividend} = \mathrm{quotient} \cdot \mathrm{divisor} + \mathrm{remainder},
\qquad \deg(\mathrm{remainder}) < \deg(\mathrm{divisor})
$$

`PolynomialDivModResult<MOD>` は次の 2 つのメンバを持つ。

- `quotient`: 商
- `remainder`: 余り

入力の末尾の $0$ は無視し、結果は正規化して返す。零多項式で割ると `std::invalid_argument` を送出する。

## 余りだけを求める

```cpp
template<int MOD>
math::Polynomial<MOD> math::polynomial_remainder(
    math::Polynomial<MOD> dividend,
    math::Polynomial<MOD> divisor
);
```

`polynomial_divmod` の余りだけを返す。例外条件は同じである。

## 時間計算量・空間計算量

$n$ を `dividend.size()`、$m$ を `divisor.size()` とし、NTT による長さ $k$ の多項式の積の計算量を $M(k)=O(k\log k)$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `polynomial_divmod` | $O(M(n))$ | $O(n)$ |
| `polynomial_remainder` | $O(M(n))$ | $O(n)$ |

小さい入力では、定数倍を抑えるために $O(nm)$ の筆算に切り替える。

## 注意点

- `divisor` の最高次の係数が逆元を持つ必要がある。
- NTT に必要な 2 の冪の変換長が $\mathrm{MOD} - 1$ を割り切る必要がある。
