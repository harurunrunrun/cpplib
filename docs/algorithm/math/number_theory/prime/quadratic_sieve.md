---
title: Quadratic Sieve Factorization (二次篩因数分解)
documentation_of: ../../../../../src/algorithm/math/number_theory/prime/quadratic_sieve.hpp
---

$x^2-n$ がfactor base上でsmoothになる関係を集め、$\mathbb F_2$ 線形従属から非自明因数を得る。

## `QuadraticSieveResult`

`factor` は見つかった因数、`tested_values` は試した $x$ の数、`smooth_relations` はsmooth関係数、`factor_base_size` はfactor baseの大きさ。素数入力では `factor == n`。

## `quadratic_sieve_factor(value, factor_base_bound=1000, maximum_tested_values=1000000, extra_relations=16)`

`factor_base_bound` 以下の素数をfactor base候補とし、$\lceil\sqrt n\rceil$ から順に関係を収集する。各素数に対する $x^2\equiv n$ の根をTonelli--Shanks法で前計算し、根の合同類だけを65536要素ずつ正確にblock sieveする。factor base上限を $P$、factor base数を $B$、試行数を $T$、smooth関係数を $R$ とすると、素数列挙は $O(P\log\log P)$、根の前計算は $O(B\log^2 P)$、篩は $O(T\log\log P)$、smooth関係の指数復元は $O(RB)$、逐次GF(2)消去は $O(RB^2/64)$。空間計算量は $O(P+B R+B^2/64+\min(T,65536))$。64-bit modular multiplicationには `unsigned __int128` を用いる。

## 注意点

確率的・資源上限制約付きの因数探索なので、上限内に十分なsmooth関係が得られない場合は `factor == nullopt`。`value < 2`、0の試行上限などには `std::invalid_argument` を送出する。

## 時間計算量

The bound is stated in the API section above.
