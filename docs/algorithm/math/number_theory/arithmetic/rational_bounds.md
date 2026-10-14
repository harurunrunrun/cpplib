---
title: Bounded Rational Bounds (有理数の上下近似) [rational_approximation]
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/rational_bounds.hpp
---

`math::rational_bounds(x, y, limit)` は正の有理数 `x/y` を、分子・分母がともに `limit` 以下の既約分数で上下から最良近似する。戻り値は `{ {下側分子,下側分母}, {上側分子,上側分母} }`。端点には `0/1` と `1/0` を用いる。すべての引数は正の `uint64_t`。

## 時間計算量

Stern--Brocot 木の連続する同方向移動を除算でまとめ、時間 $O(\log(x+y))$、追加領域 $O(1)$。比較には `__uint128_t` を使い、64 bit の積のオーバーフローを避ける。

4引数の `rational_bounds(x,y,numerator_limit,denominator_limit)` では分子と分母へ別の上限を指定できる。

類題: 分子と分母で異なる予算を与えた最良近似を求める。下側・上側の一方だけも利用できる。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
