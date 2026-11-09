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

入力を `NormalizedConvexPolygon` に変換し、正規化済み多角形版に処理を委ねる。
返す添字は、正規化後の頂点列 `NormalizedConvexPolygon::vertices()` に対するものである。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 頂点列版 | $O(N)$ | $O(N)$ |

## 注意点

入力は凸多角形の弱凸な周回列でなければならない。非凸な入力には `std::invalid_argument` を送出する。空の入力では `CONVEX_POLYGON_NPOS` を返す。
同じ多角形に多数のクエリを投げる場合は、`NormalizedConvexPolygon` を一度だけ構築して正規化済み多角形版を使うと、1 回あたり $O(\log N)$ になる。
