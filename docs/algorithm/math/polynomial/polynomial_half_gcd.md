---
title: Polynomial Half-GCD (多項式Half-GCD)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_half_gcd.hpp
---

体上の多項式のユークリッド互除法を、多項式を成分とする $2 \times 2$ 行列と分割統治によりまとめて進める（half-GCD）。既存の高速な多項式除算・畳み込みと組み合わせ、最大公約数と Bézout 係数を準線形時間で求める。以下、`first` を $A$、`second` を $B$ とする。すべて `math::` 名前空間にある。

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

を満たす Bézout 係数を保持する。

## `apply_polynomial_matrix`

```cpp
template<int MOD>
std::pair<Polynomial<MOD>, Polynomial<MOD>> apply_polynomial_matrix(
    const PolynomialMatrix2x2<MOD>& matrix,
    const Polynomial<MOD>& first,
    const Polynomial<MOD>& second
)
```

行列とベクトルの積 $\mathrm{matrix} \cdot (\mathrm{first}, \mathrm{second})^T$ を返す。

## `polynomial_half_gcd`

```cpp
template<int MOD>
PolynomialMatrix2x2<MOD> polynomial_half_gcd(
    Polynomial<MOD> first,
    Polynomial<MOD> second
)
```

$\deg A>\deg B$（$B$ は零多項式でもよい）に対し、`apply_polynomial_matrix` を適用した後の第 2 成分の次数が $\lceil\deg A/2\rceil$ 未満になるまでユークリッド互除法を進めた、ユニモジュラ行列を返す。第 2 成分がすでにその次数未満なら単位行列を返す。

## `polynomial_extended_gcd_half`

```cpp
template<int MOD>
PolynomialExtendedGcdResult<MOD> polynomial_extended_gcd_half(
    Polynomial<MOD> first,
    Polynomial<MOD> second
)
```

monic な最大公約数と Bézout 係数を返す。両方が零多項式なら 3 つとも零多項式とする。

## `polynomial_gcd_half`

```cpp
template<int MOD>
Polynomial<MOD> polynomial_gcd_half(
    Polynomial<MOD> first,
    Polynomial<MOD> second
)
```

monic な最大公約数だけを返す。

## 時間計算量・空間計算量

$N=\max(\deg A,\deg B)+1$、次数 $N$ の多項式の乗算の時間を $M(N)$ とする。

| 操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `apply_polynomial_matrix` | $O(M(N))$ | $O(N)$ |
| `polynomial_half_gcd` | $O(M(N)\log N)$ | $O(N\log N)$ |
| `polynomial_extended_gcd_half` | $O(M(N)\log N)$ | $O(N\log N)$ |
| `polynomial_gcd_half` | $O(M(N)\log N)$ | $O(N\log N)$ |

同梱の NTT を使える法では $M(N)=O(N\log N)$ である。

## 注意点

- `MOD` は素数を想定する。零でない最高次の係数の逆元が必要である。
- 係数の列の末尾の $0$ は自動的に取り除く。零多項式は空の `std::vector` で表す。
- `polynomial_half_gcd` は $A$ が零多項式の場合、または $B \ne 0$ かつ $\deg A \le \deg B$ の場合に `std::invalid_argument` を送出する。
- 大きな次数では、法が内部の NTT の長さを許容する必要がある。
