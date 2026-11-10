---
title: Maximum Distance between General Polygons (一般多角形間の最大距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set_point_set/polygon_maximum_distance.hpp
---

凸性を仮定しない 2 つの多角形について、それぞれの閉領域に属する点の対の最大ユークリッド距離を返す。
最大距離は両者の凸包の頂点対で達成されるので、各入力の凸包を求めてから凸多角形間の最大距離（`convex_polygon_maximum_distance`）を計算する。

## API

```cpp
long double polygon_maximum_distance(
    std::vector<Point> first,
    std::vector<Point> second
);
```

## 時間計算量

頂点数を $N, M$、凸包の頂点数を $H_1, H_2$ とする。

- 時間計算量: $O(N \log (N+1) + M \log (M+1) + H_1 + H_2)$
- 追加空間計算量: $O(N + M)$

## 注意点

- 頂点の順序、凸性、自己交差の有無を問わず、入力頂点集合の凸包どうしの最大距離として計算する。
- どちらかの入力が空なら `std::invalid_argument` を送出する。
