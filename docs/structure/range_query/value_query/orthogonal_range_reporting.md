---
title: Orthogonal Range Reporting (直交範囲列挙)
documentation_of: ../../../../src/structure/range_query/value_query/orthogonal_range_reporting.hpp
---

二次元点集合から半開長方形内の入力添字を列挙する。

## API

- `OrthogonalRangeReporting(points)`: 点集合を構築する。
- `size()`: 点数を返す。
- `report(lower_x,upper_x,lower_y,upper_y)`: 条件を満たす入力添字を返す。

## 時間計算量

- 構築: $O(N\log N)$。
- `report`: $O(\log^2 N+K)$。
- `size`: $O(1)$。

## 注意点

- 同一点も別の入力添字として保持する。出力順は保証しない。
