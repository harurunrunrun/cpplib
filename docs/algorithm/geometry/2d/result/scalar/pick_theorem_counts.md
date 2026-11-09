---
title: Pick Theorem Lattice Point Counts (ピックの定理による格子点数)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/scalar/pick_theorem_counts.hpp
---

単純な格子多角形について、境界上と内部の格子点の個数を計算する。

## API

```cpp
template<std::integral Coordinate>
PickTheoremResult pick_theorem(
    const std::vector<IntegerPoint<Coordinate>>& polygon
);
```

各辺上の格子点数と符号付き面積の 2 倍から Pick の定理を適用し、境界上の点数 `boundary_points` と内部の点数 `interior_points`（いずれも `__uint128_t`）を返す。

## 時間計算量

頂点数を $N$、座標差の最大絶対値を $C$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `pick_theorem(polygon)` | $O(N \log C)$ | $O(1)$ |

## 注意点

入力は 3 頂点以上の単純な格子多角形でなければならない。座標型は 64 bit 以下の整数型を想定する。例外の条件は `lattice_polygon_interior_points` と同じである。
