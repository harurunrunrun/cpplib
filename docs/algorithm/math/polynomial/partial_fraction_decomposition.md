---
title: Polynomial Partial Fraction Decomposition (多項式部分分数分解)
documentation_of: ../../../../src/algorithm/math/polynomial/partial_fraction_decomposition.hpp
---

多項式 $P$ と、互いに素な正の次数の多項式 $q_i$ の積 $Q=\prod_i q_i$ に対して

$$
\frac{P}{Q}=A+\sum_i\frac{r_i}{q_i},\qquad \deg r_i<\deg q_i
$$

を求める。各 $r_i$ は中国剰余定理により $r_i=(P\bmod q_i)(Q/q_i)^{-1}\bmod q_i$ と計算する（$P \bmod q_i$ は $P \bmod Q$ から求める）。同じ根を持つ重複した因子は、1 つの冪 $q_i=(x-a)^k$ にまとめて渡せばよい。

## 型

```cpp
template<int MOD>
struct PartialFractionDecompositionResult{
    Polynomial<MOD> polynomial_part;
    std::vector<Polynomial<MOD>> numerators;
    std::vector<Polynomial<MOD>> denominator_factors;
};
```

係数の列は次数の昇順で、空の列は零多項式を表す。

## `partial_fraction_decomposition`

```cpp
template<int MOD>
PartialFractionDecompositionResult<MOD> partial_fraction_decomposition(
    Polynomial<MOD> numerator,
    std::vector<Polynomial<MOD>> denominator_factors
)
```

`polynomial_part` が多項式部分 $A$、`numerators[i] / denominator_factors[i]` が第 $i$ 項である。因子の列が空なら `polynomial_part == numerator` で、分数部分は空になる。

## 時間計算量・空間計算量

分母の次数の合計を $D$、因子の個数を $K$、$d_i = \deg q_i$ とし、次数 $D$ の多項式の乗算・除算の計算量をそれぞれ $M(D)$, $R(D)$ とすると:

- 時間計算量: $O(K(M(D)+R(D)) + \sum_i d_i^2)$（最後の項は各因子での拡張ユークリッド互除法）
- 追加の空間計算量: $O(KD)$（戻り値を含む）

## 注意点

- 係数が `MOD` を法として体をなすこと（`MOD` が素数であること）、および `denominator_factors` のどの 2 つも互いに素であることを要求する。
- 次数が $0$ 以下の因子や、互いに素でない因子があると `std::runtime_error` を送出する。
- 係数の逆元が存在しない合成数の法では、`Modint::inv` が例外を送出する。
