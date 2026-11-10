---
title: Polynomial Factorization over Finite Fields (有限体上多項式因数分解)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_factorization_finite_field.hpp
---

有限素体上の非零多項式を、monic な既約因子とその重複度に分解する。平方因子分解、次数別分解、Cantor–Zassenhaus 法による等次数分解を順に行う。

## API

- `math::polynomial_factorization_finite_field<MOD>(polynomial)`: 最高次の係数と既約因子の列を `FiniteFieldPolynomialFactorization<MOD>` で返す。
- `FiniteFieldPolynomialFactorization<MOD>::leading_coefficient`: 入力の最高次の係数。
- `FiniteFieldPolynomialFactorization<MOD>::factors`: 因子と重複度の配列。
- `FiniteFieldPolynomialFactor<MOD>::polynomial`: 最高次の係数を $1$ に正規化した既約因子。
- `FiniteFieldPolynomialFactor<MOD>::multiplicity`: その因子の重複度。

## 時間計算量・空間計算量

入力の次数を $n$、法多項式のもとでの乗算・剰余の時間を $R(n)$ とする。平方因子分解と次数別分解は $O(n \log \mathrm{MOD} \cdot R(n))$ 時間、等次数分解は期待 $O(n \log \mathrm{MOD} \cdot R(n))$ 時間。追加領域は、因子の出力を含めた $O(n)$ に、下位の多項式の積・half-GCD が使う作業領域を加えたものである。

## 注意点

- `MOD` は素数でなければならない（`static_assert` で検査する）。
- 零多項式は因数分解を一意に定められないので、`std::invalid_argument` を送出する。定数多項式では最高次の係数だけを返し、因子の列は空になる。
- 乱択の分割には固定状態の xorshift を使うので結果は再現できる。正しさは乱数の値に依存しない。
