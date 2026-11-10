---
title: Point Set Maximum Area Triangle (点集合内最大面積三角形)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/point_set_maximum_area_triangle.hpp
---

凸性や頂点の順序を仮定せず、入力点から選べる最大面積の三角形を返す。最適解が凸包の頂点で達成される性質を利用する。

## API

```cpp
PointSetMaximumAreaTriangleResult point_set_maximum_area_triangle(
    std::vector<Point> points
);
```

選んだ 3 点 `vertices`、面積の 2 倍 `doubled_area`、有効フラグ `valid` を返す。面積は `area()` で得られる。非退化な 3 点が存在しなければ `valid == false` となる。

## 時間計算量

入力点数を $N$、凸包の頂点数を $H$ とする。

- 凸包の構築: $O(N \log N)$
- 最大三角形の探索: $O(H^2)$
- 追加空間計算量: $O(N)$

## 注意点

入力座標は有限である必要がある。多角形の境界に限らず、任意の順序の点集合に使用できる。
