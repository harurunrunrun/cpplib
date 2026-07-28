---
title: Convex Polygon Minimum Width from Points (頂点列からの凸多角形最小幅)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/convex_polygon_minimum_width_points.hpp
---

凸多角形の周回頂点列を正規化して最小幅を求める。

## API

```cpp
long double convex_polygon_minimum_width(std::vector<Point> polygon);
```

入力を `NormalizedConvexPolygon` に変換して正規化済みoverloadへ委譲する。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 頂点列overload | $O(N)$ | $O(N)$ |

## 注意点

入力は凸多角形の弱凸な周回列でなければならない。非凸入力には
`std::invalid_argument` を送出する。点または線分へ退化した入力では0を返す。
