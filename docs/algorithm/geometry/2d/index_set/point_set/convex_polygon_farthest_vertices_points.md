---
title: Convex Polygon Farthest Vertices from Points (頂点列からの凸多角形最遠頂点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/index_set/point_set/convex_polygon_farthest_vertices_points.hpp
---

凸多角形の周回頂点列を正規化し、各頂点に対する最遠頂点を返す。

## API

```cpp
std::vector<std::size_t> convex_polygon_farthest_vertices(
    std::vector<Point> polygon
);
```

入力を `NormalizedConvexPolygon` に変換し、正規化済み多角形版に処理を委ねる。
返す列の添字・値はどちらも、正規化後の頂点列 `NormalizedConvexPolygon::vertices()` に対するものである。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 頂点列版 | $O(N)$ | $O(N)$ |

## 注意点

入力は凸多角形の弱凸な周回列でなければならない。非凸な入力には `std::invalid_argument` を送出する。
重複点と同一直線上の中間点は正規化で取り除かれる。入力位置ごとの結果が必要な場合は `integer_convex_farthest_neighbors` を使う。
