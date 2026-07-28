---
title: Convex Polygon Support Vertex from Points (頂点列からの凸多角形支持点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/index/point_set_point/convex_polygon_support_vertex_points.hpp
---

凸多角形の周回頂点列を正規化し、指定方向の支持点を求める。

## API

```cpp
std::size_t convex_polygon_support_vertex(
    std::vector<Point> polygon,
    const Point& direction
);
```

入力を `NormalizedConvexPolygon` に変換して正規化済みoverloadへ委譲する。
返す添字は正規化後の頂点列に対する。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 頂点列overload | $O(N)$ | $O(N)$ |

## 注意点

入力は凸多角形の弱凸な周回列でなければならない。非凸入力には
`std::invalid_argument` を送出する。空入力では `CONVEX_POLYGON_NPOS` を返す。
