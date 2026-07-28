---
title: Line-Convex Polygon Intersection from Points (頂点列による直線と凸多角形の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/line_point_set/intersect_line_convex_polygon_points.hpp
---

凸多角形の周回頂点列を正規化し、無限直線との交差を判定する。

## API

```cpp
bool intersect_line_convex_polygon(
    const Line& line,
    std::vector<Point> polygon
);
```

入力を `NormalizedConvexPolygon` に変換して正規化済みoverloadへ委譲する。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 頂点列overload | $O(N)$ | $O(N)$ |

## 注意点

入力は凸多角形の弱凸な周回列でなければならない。非凸入力または退化直線には
`std::invalid_argument` を送出する。空入力にはfalseを返す。
