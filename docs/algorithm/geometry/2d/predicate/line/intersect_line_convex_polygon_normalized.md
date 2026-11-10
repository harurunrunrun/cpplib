---
title: Line Intersection with a Normalized Convex Polygon (直線と正規化凸多角形の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/line/intersect_line_convex_polygon_normalized.hpp
---

無限直線と正規化済みの閉凸集合が共通点を持つか判定する。

## API

```cpp
bool intersect_line_convex_polygon(
    const Line& line,
    const NormalizedConvexPolygon& polygon
);
```

直線の法線方向について最大・最小となる支持頂点を二分探索で求め、その 2 頂点が直線をまたぐ（または直線上にある）かを調べる。接する場合も交差に含める。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `intersect_line_convex_polygon(line, polygon)` | $O(\log N)$ | $O(1)$ |

## 注意点

- `line` の方向ベクトルの長さが `GEOMETRY_EPS` 以下の退化直線には `std::invalid_argument` を送出する。
- 空の凸集合には `false` を返す。
- `polygon` は有効な `NormalizedConvexPolygon` でなければならない。
