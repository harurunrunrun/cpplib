---
title: Priority Search Tree (優先探索木)
documentation_of: ../../../../src/structure/range_query/value_query/priority_search_tree.hpp
---

$x$ の範囲と $y$ の上限を指定する三辺領域列挙用の静的優先探索木。

## API

- `Point`: `x`, `y`, `payload` を保持する。
- `PrioritySearchTree(points)`: 点集合を構築する。
- `size()`: 点数を返す。
- `report(lower_x,upper_x,upper_y)`: `lower_x <= x < upper_x`, `y < upper_y` のpayloadを返す。

## 時間計算量

- 構築: $O(N\log^2 N)$。
- `report`: $O(\log N+K)$。
- `size`: $O(1)$。

## 注意点

- 出力順は保証しない。
