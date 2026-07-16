---
title: Polynomial Half-GCD (多項式Half-GCD)
documentation_of: ../src/algorithm/math/polynomial_half_gcd.hpp
---

体上の多項式Euclid互除法を、2次正方多項式行列と分割統治によりまとめて
進める。既存の高速多項式除算・畳み込みと組み合わせ、最大公約数とBezout
係数を準線形時間で求める。

## 型

```cpp
template<int MOD>
struct PolynomialMatrix2x2{
    Polynomial<MOD> top_left, top_right;
    Polynomial<MOD> bottom_left, bottom_right;
};
```

多項式を成分とする $2\times2$ 行列である。

```cpp
template<int MOD>
struct PolynomialExtendedGcdResult{
    Polynomial<MOD> gcd;
    Polynomial<MOD> left_coefficient;
    Polynomial<MOD> right_coefficient;
};
```

`gcd` と、

$$
\mathtt{left\_coefficient}\,A+
\mathtt{right\_coefficient}\,B=\mathtt{gcd}
$$

を満たすBezout係数を保持する。

## `apply_polynomial_matrix`

```cpp
template<int MOD>
pair<Polynomial<MOD>, Polynomial<MOD>> apply_polynomial_matrix(
    const PolynomialMatrix2x2<MOD>& matrix,
    const Polynomial<MOD>& first,
    const Polynomial<MOD>& second
)
```

`matrix * (first, second)^T` を返す。

## `polynomial_half_gcd`

```cpp
template<int MOD>
PolynomialMatrix2x2<MOD> polynomial_half_gcd(
    Polynomial<MOD> first,
    Polynomial<MOD> second
)
```

$\deg A>\deg B$ に対し、`apply_polynomial_matrix` 後の第2多項式の次数が
$\lceil\deg A/2\rceil$ 未満になるまでEuclid互除法をまとめたユニモジュラ行列を
返す。第2多項式が既にその次数未満なら単位行列を返す。

## `polynomial_extended_gcd_half`

```cpp
template<int MOD>
PolynomialExtendedGcdResult<MOD> polynomial_extended_gcd_half(
    Polynomial<MOD> first,
    Polynomial<MOD> second
)
```

monicな最大公約数とBezout係数を返す。両方が零多項式なら3要素とも零多項式
とする。

## `polynomial_gcd_half`

```cpp
template<int MOD>
Polynomial<MOD> polynomial_gcd_half(
    Polynomial<MOD> first,
    Polynomial<MOD> second
)
```

monicな最大公約数だけを返す。

## 時間計算量

$N=\max(\deg A,\deg B)+1$、次数 $N$ の多項式乗算時間を $M(N)$ とする。

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `apply_polynomial_matrix` | $O(M(N))$ | $O(N)$ |
| `polynomial_half_gcd` | $O(M(N)\log N)$ | $O(N\log N)$ |
| `polynomial_extended_gcd_half` | $O(M(N)\log N)$ | $O(N\log N)$ |
| `polynomial_gcd_half` | $O(M(N)\log N)$ | $O(N\log N)$ |

同梱のNTTを使える法では $M(N)=O(N\log N)$ である。

## 注意点

- `MOD` は素数を想定する。零でない先頭係数の逆元が必要である。
- 係数列末尾の0は自動的に除かれる。零多項式は空のvectorで表す。
- `polynomial_half_gcd` の次数条件に違反すると `invalid_argument` を送出する。
- 大きな次数では、法が内部のNTT長を許容する必要がある。
