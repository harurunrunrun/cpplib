---
title: Polygon Support Vertex (一般多角形の支持点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/index/point_set_point/polygon_support_vertex.hpp
---

凸性を仮定しない多角形または点列から、指定方向との内積が最大の頂点を返す。

## API

```cpp
std::size_t polygon_support_vertex(
    const std::vector<Point>& polygon,
    const Point& direction
);
```

最大値を取る頂点が複数ある場合は最小添字を返す。空入力では
`POLYGON_SUPPORT_VERTEX_NPOS` を返す。

## 時間計算量

頂点数を $N$ とする。

- 時間計算量: $O(N)$
- 追加空間計算量: $O(1)$

## 注意点

頂点順や凸性は問わない。座標と方向は有限でなければならず、違反時は
`std::invalid_argument` を送出する。多数queryには凸包を構築して
`convex_polygon_support_vertex` を使う方が高速である。
