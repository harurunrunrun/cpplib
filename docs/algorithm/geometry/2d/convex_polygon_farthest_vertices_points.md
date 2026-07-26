---
title: Convex Polygon Farthest Vertices from Points (頂点列からの凸多角形最遠頂点)
documentation_of: ../../../../src/algorithm/geometry/2d/convex_polygon_farthest_vertices_points.hpp
---

凸多角形の周回頂点列を正規化し、各頂点に対する最遠頂点を返す。

## API

```cpp
std::vector<std::size_t> convex_polygon_farthest_vertices(
    std::vector<Point> polygon
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
`std::invalid_argument` を送出する。重複点と同一直線上の中間点は正規化で除かれる。
