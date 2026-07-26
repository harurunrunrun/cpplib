---
title: Line-Convex Polygon Intersection Aggregator (直線と凸多角形の交差判定集約ヘッダ)
documentation_of: ../../../../src/algorithm/geometry/2d/intersect_line_convex_polygon.hpp
---

正規化済み凸多角形入力と頂点列入力の交差判定APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するoverload |
| --- | --- |
| `intersect_line_convex_polygon_normalized.hpp` | `intersect_line_convex_polygon(line, const NormalizedConvexPolygon&)` |
| `intersect_line_convex_polygon_points.hpp` | `intersect_line_convex_polygon(line, std::vector<Point>)` |

## 集約されるAPI

```cpp
bool intersect_line_convex_polygon(
    const Line& line,
    const NormalizedConvexPolygon& polygon
);
bool intersect_line_convex_polygon(
    const Line& line,
    std::vector<Point> polygon
);
```

無限直線と閉凸集合が接触を含む共通点を持つか判定する。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。

| overload | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `NormalizedConvexPolygon` | $O(\log N)$ | $O(1)$ |
| `std::vector<Point>` | $O(N)$ | $O(N)$ |

## 注意点

退化直線には `std::invalid_argument` を送出する。空の凸集合にはfalseを返す。
頂点列overloadへの非凸入力には `std::invalid_argument` を送出する。
