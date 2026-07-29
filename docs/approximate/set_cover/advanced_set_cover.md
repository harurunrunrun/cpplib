---
title: Advanced Set Cover and Rounding (高度な集合被覆と丸め)
documentation_of: ../../../src/approximate/set_cover/advanced_set_cover.hpp
---

`approximate::set_cover` 名前空間で weighted set cover の primal-dual /
local-ratio 法と fractional rounding を提供する。

## Primal-Dual / Local Ratio

```cpp
primal_dual_set_cover(universe_size, sets, weights);
local_ratio_set_cover(universe_size, sets, weights);
```

Primal-Dual は未被覆要素の dual を、incident set が tight になるまで増加させ、
tight set を選ぶ。`PrimalDualSetCoverResult` は `cover` と
`element_dual` を返す。Local Ratio は同じ要素に接続する residual weight から
最小値を引き、weight 0 になった set を選ぶ。両者とも最後に reverse-delete で
冗長 set を除く。

set 内要素出現総数を $L$ とすると、正規化を含む時間計算量は
$O(L\log L)$、空間計算量は $O(L+U+S)$。最大 element frequency を $f$ とする
標準仮定で $f$ 近似である。

## Set-cover rounding

```cpp
randomized_rounding_set_cover(
    universe_size, sets, fractional, random, rounds = 1);
deterministic_rounding_set_cover(universe_size, sets, fractional);
iterative_rounding_set_cover(universe_size, sets, fractional);
lp_rounding_set_cover(universe_size, sets, fractional);
```

Randomized Rounding は各 round で未選択 set を確率 $x_i$ で独立に選ぶため、
未被覆要素を返す場合がある。Deterministic / LP Rounding は feasible LP 解を
$1/f$ threshold で丸める。Iterative Rounding は未被覆要素を順に取り、それを
覆う最大 fractional value の set を固定する。

Randomized は時間計算量 $O(RS+L)$、Deterministic/LP は $O(S+L)$、
Iterative は worst-case $O(US+L)$。空間計算量はいずれも $O(U+S+L)$。

## Dependent / Pipage Rounding

```cpp
dependent_rounding(fractional, random);
pipage_rounding(fractional, objective);
```

Dependent Rounding は fractional 変数対を和と各変数の期待値を保つ二択更新で
0/1 化する。Pipage Rounding は同じ和保存方向の二端点を `objective(vector)`
で評価し、大きい側を決定的に選ぶ。返値は `vector<unsigned char>`。

変数数を $S$、objective 1回の計算量を $Q$ とすると、Dependent Rounding は
時間計算量 $O(S^2)$、Pipage Rounding は $O(S^2+SQ)$。空間計算量は $O(S)$。

## 注意点

集合要素は `[0, universe_size)`、weight は非負有限、fractional value は有限な
`[0,1]` でなければならない。Deterministic/Iterative/LP は各要素で
$\sum_{i:e\in S_i}x_i\ge1$ の feasible 解を要求する。Randomized Rounding の
coverage は確率的であり、round 数を増やしても完全被覆を保証しない。Pipage の
近似保証は渡した objective が必要な凸性・concavity 条件を満たす場合に限る。
