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

直線方向の法線に対する最大・最小支持点を二分探索し、直線を跨ぐか調べる。
接触も交差に含める。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 正規化済みoverload | $O(\log N)$ | $O(1)$ |

## 注意点

- `line.a == line.b` と判定される退化直線には `std::invalid_argument` を送出する。
- 空の凸集合にはfalseを返す。
- `polygon` は有効な `NormalizedConvexPolygon` でなければならない。
