---
title: Point Set Global Tangent Points (点集合への大域接点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set_point/point_set_tangent_points.hpp
---

凸性や点の順序を仮定しない点集合に対し、外部の点から引いた 2 本の大域的な支持接線の接点を返す。対象は、全入力点を同じ閉半平面に含む接線である。

## API

```cpp
std::optional<std::array<Point, 2>> point_set_tangent_points(
    std::vector<Point> points,
    const Point& source
);
```

左右の接点の座標を返す。点集合が空なら `std::nullopt` を返す。

## 時間計算量

点数を $N$、凸包の頂点数を $H$ とする。

- 凸包の構築: $O(N \log N)$
- 接点の探索: $O(\log H)$
- 追加空間計算量: $O(N)$

## 注意点

`source` は点集合の凸包の外部になければならず、内部または境界上にあれば `std::invalid_argument` を送出する。凹頂点に対する局所的な接線ではなく、点集合全体の支持線を返す。
