---
title: ±1 RMQ (±1区間最小値)
documentation_of: ../../../../src/structure/range_query/extrema/plus_minus_one_rmq.hpp
---

隣接差が常に $+1$ または $-1$ の整数列に特化した静的RMQ。

## API

- `PlusMinusOneRMQ(values)`: 列を前処理する。
- `size()`: 長さを返す。
- `argmin(left,right)`: 最小値の左端添字を返す。
- `minimum(left,right)`: 最小値を返す。

## 時間計算量

- 構築: $O(N\log N)$（同じブロック型の表は共有）。
- `size`, `argmin`, `minimum`: $O(1)$。

## 注意点

- 各隣接差が厳密に `1` または `-1` でなければ例外を送出する。
