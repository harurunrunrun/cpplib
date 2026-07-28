---
title: Polynomial Factorization over Finite Fields (有限体上多項式因数分解)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_factorization_finite_field.hpp
---

有限素体上の非零多項式を、monic既約因子と重複度へ分解する。平方因子分解、
次数別分解、Cantor--Zassenhaus等次数分解を順に行う。

## API

- `polynomial_factorization_finite_field<MOD>(polynomial)`: 最高次係数と既約因子列を返す。
- `FiniteFieldPolynomialFactorization<MOD>::leading_coefficient`: 入力の最高次係数。
- `FiniteFieldPolynomialFactorization<MOD>::factors`: 因子と重複度の配列。
- `FiniteFieldPolynomialFactor<MOD>::polynomial`: 最高次係数を1に正規化した既約因子。
- `FiniteFieldPolynomialFactor<MOD>::multiplicity`: その因子の重複度。

## API別の時間計算量・空間計算量

入力次数を $n$、法多項式の下での乗算・剰余時間を $R(n)$ とする。
平方因子分解と次数別分解は $O(n\log MOD\,R(n))$ 時間、等次数分解は
期待 $O(n\log MOD\,R(n))$ 時間。追加領域は因子出力を含め $O(n)$ に加え、
下位の多項式積・half-GCDが使う作業領域。

## 注意点

`MOD` は素数でなければならない。零多項式は因数分解を一意に定められないため
`std::invalid_argument`。定数多項式では最高次係数だけを返し、因子列は空。
乱択分割は再現可能な固定状態のxorshiftを使い、正しさは乱数値に依存しない。
