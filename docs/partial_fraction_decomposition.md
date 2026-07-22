---
title: Polynomial Partial Fraction Decomposition (多項式部分分数分解)
documentation_of: ../src/algorithm/math/polynomial/partial_fraction_decomposition.hpp
---

多項式 $P$ と、互いに素な正次数多項式 $q_i$ の積
$Q=\prod_i q_i$ に対して

$$
\frac{P}{Q}=A+\sum_i\frac{r_i}{q_i},\qquad \deg r_i<\deg q_i
$$

を求める。各 $r_i$ は中国剰余定理により
$r_i=(P\bmod q_i)(Q/q_i)^{-1}\bmod q_i$ と計算する。同じ根の重複因子は
一つの冪 $q_i=(x-a)^k$ にまとめればよい。

## 型

```cpp
template<int MOD>
struct PartialFractionDecompositionResult{
    Polynomial<MOD> polynomial_part;
    std::vector<Polynomial<MOD>> numerators;
    std::vector<Polynomial<MOD>> denominator_factors;
};
```

係数vectorは次数昇順。空vectorは0多項式。

## `partial_fraction_decomposition`

```cpp
template<int MOD>
PartialFractionDecompositionResult<MOD> partial_fraction_decomposition(
    Polynomial<MOD> numerator,
    std::vector<Polynomial<MOD>> denominator_factors
)
```

`numerators[i] / denominator_factors[i]` が第 $i$ 項。因子が空なら
`polynomial_part == numerator`、分数部分は空。

全分母次数を $D$、因子数を $K$ とし、多項式乗算・除算の計算量をそれぞれ
$M(D)$, $R(D)$ とすると:

- 時間計算量: $O(K(M(D)+R(D)))$
- 追加空間計算量: $O(KD)$（返却値を含む）

## 注意点

`MOD` 上の係数が体をなすこと、および各 `denominator_factors` がpairwise coprime
であることを要求する。0次以下の因子、互いに素でない因子では `runtime_error`。
係数の逆元が存在しない合成数modでも `Modint::inv` が例外を送出する。
