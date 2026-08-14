---
title: Minimum Distance between General Polygons (一般多角形間の最小距離)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set_point_set/polygon_minimum_distance.hpp
---

凸性を仮定しない2つの単純多角形の閉領域間の最小Euclidean距離を返す。
領域が交差または包含する場合は $0$ を返す。境界間距離はAABB階層で探索する。

## API

```cpp
long double polygon_minimum_distance(
    const std::vector<Point>& first,
    const std::vector<Point>& second
);
```

## 時間計算量

頂点数を $N,M$ とする。

- AABB階層の構築: $O(N\log N+M\log M)$
- 距離探索: 通常は空間分割により枝刈りされる。最悪時間計算量は $O(NM)$
- 追加空間計算量: $O(N+M)$

## 注意点

各入力は頂点を境界順に並べた有限座標の単純多角形とする。頂点数1は点、頂点数2は線分として扱う。
空入力には `std::invalid_argument` を送出する。自己交差多角形の内部は対象外である。
