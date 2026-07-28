---
title: Range Tree (レンジ木)
documentation_of: ../../../../src/structure/range_query/value_query/range_tree.hpp
---

静的二次元点集合に対する直交長方形列挙を行う二層レンジ木。

## API

- `Point`: `x`, `y`, `payload` を保持する。
- `RangeTree(points)`: 点集合を構築する。
- `size()`: 点数を返す。
- `report(lower_x,upper_x,lower_y,upper_y)`: 半開長方形内のpayloadを返す。

## 時間計算量

- 構築: $O(N\log N)$ 時間・空間。
- `report`: $O(\log^2 N+K)$。
- `size`: $O(1)$。

## 注意点

- $K$ は報告点数。出力順は保証しない。
