---
title: Bounded Rational Bounds (有理数の上下近似) [rational_approximation]
documentation_of: ../../../../../src/algorithm/math/number_theory/arithmetic/rational_bounds.hpp
---

正の有理数 $x/y$ を、分子・分母に上限を課した既約分数で下側・上側から最良近似する。

## API

- `math::rational_bounds(x, y, limit)`: 分子・分母がともに `limit` 以下の既約分数のうち、$x/y$ 以下で最大のものと $x/y$ 以上で最小のものを返す。
- `math::rational_bounds(x, y, numerator_limit, denominator_limit)`: 分子の上限を `numerator_limit`、分母の上限を `denominator_limit` として別々に指定する版。

引数はすべて `std::uint64_t`。戻り値は `{ {下側の分子, 下側の分母}, {上側の分子, 上側の分母} }` で、該当する分数がない側には番兵 `0/1`（$0$）または `1/0`（$+\infty$）が入る。$x/y$ 自体が上限内の分数と一致する場合は、下側・上側とも同じ分数を返す。下側・上側の一方だけを使ってもよい。

## 時間計算量・空間計算量

Stern–Brocot 木を同じ方向へ連続して進む移動を除算でまとめて行うので、時間 $O(\log(x+y))$、追加領域 $O(1)$。比較には `__uint128_t` を使い、64 bit 整数の積のオーバーフローを避ける。

## 注意点

- 引数のいずれかが $0$ の場合は `std::invalid_argument` を送出する。
- GCC 13 / C++20 を想定する（`__uint128_t` を使用）。
