---
title: Maximum Distance between General Polygons (一般多角形間の最大距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set_point_set/polygon_maximum_distance.hpp
---

凸性を仮定しない2多角形の閉領域に属する点対の最大Euclidean距離を返す。
最大距離は両凸包の頂点対で達成される。

## API

```cpp
long double polygon_maximum_distance(
    std::vector<Point> first,
    std::vector<Point> second
);
```

## 時間計算量

頂点数を $N,M$、凸包頂点数を $H_1,H_2$ とする。

- 時間計算量: $O(N\log(N+1)+M\log(M+1)+H_1+H_2)$
- 追加空間計算量: $O(N+M)$

## 注意点

頂点順、凸性、自己交差の有無を問わず、入力頂点集合の凸包間最大距離として計算する。
空入力には `std::invalid_argument` を送出する。
