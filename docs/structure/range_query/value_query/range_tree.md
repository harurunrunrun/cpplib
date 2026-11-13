---
title: Range Tree (レンジ木)
documentation_of: ../../../../src/structure/range_query/value_query/range_tree.hpp
---

静的な 2 次元の点集合に対して、直交する長方形内の点を列挙する 2 層のレンジ木である。

## API

```cpp
template<class Coordinate, class Payload = int>
class RangeTree;
```

- `Point{x, y, payload}`: 1 点の座標と、列挙時に返す値を保持する。
- `RangeTree()`: 空の木を構築する。
- `explicit RangeTree(vector<Point> points)`: 点集合から構築する。
- `size()`: 点の個数を返す。
- `report(lower_x, upper_x, lower_y, upper_y)`: 半開の長方形 `[lower_x, upper_x) x [lower_y, upper_y)` 内の点の `payload` を返す。

## 時間計算量

$N$ を点の個数、$K$ を報告する点の個数とする。

- 構築: $O(N\log N)$ 時間・空間
- `report`: $O(\log^2 N+K)$
- `size`: $O(1)$

## 注意点

- 出力の順序は保証しない。
- `upper_x < lower_x` または `upper_y < lower_y` なら `std::invalid_argument` を送出する。
- `Coordinate` には `operator<` と `operator!=` が必要である。
