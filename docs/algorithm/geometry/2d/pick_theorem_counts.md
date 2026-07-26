---
title: Pick Theorem Lattice Point Counts (ピックの定理による格子点数)
documentation_of: ../../../../src/algorithm/geometry/2d/pick_theorem_counts.hpp
---

単純格子多角形の境界上と内部の格子点数を計算する。

## API

```cpp
template<std::integral Coordinate>
PickTheoremResult pick_theorem(
    const std::vector<IntegerPoint<Coordinate>>& polygon
);
```

各辺上の格子点数と符号付き倍面積からPickの定理を適用し、両点数を返す。

## 時間計算量

頂点数を $N$、座標差の最大絶対値を $C$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `pick_theorem(polygon)` | $O(N\log C)$ | $O(1)$ |

## 注意点

入力は3頂点以上の単純格子多角形でなければならない。
座標型は64-bit以下の整数型を想定し、例外条件は `lattice_polygon_interior_points` と同じである。
