---
title: Convex Polygon Minimum Width Aggregator (凸多角形の最小幅集約ヘッダ)
documentation_of: ../../../../src/algorithm/geometry/2d/convex_polygon_minimum_width.hpp
---

正規化済み凸多角形入力と頂点列入力の最小幅APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するoverload |
| --- | --- |
| `convex_polygon_minimum_width_normalized.hpp` | `convex_polygon_minimum_width(const NormalizedConvexPolygon&)` |
| `convex_polygon_minimum_width_points.hpp` | `convex_polygon_minimum_width(std::vector<Point>)` |

## 集約されるAPI

```cpp
long double convex_polygon_minimum_width(
    const NormalizedConvexPolygon& polygon
);
long double convex_polygon_minimum_width(std::vector<Point> polygon);
```

平行な2支持直線で凸集合を挟む幅の最小値を返す。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。

| overload | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `NormalizedConvexPolygon` | $O(N)$ | $O(1)$ |
| `std::vector<Point>` | $O(N)$ | $O(N)$ |

## 注意点

点または線分へ退化した凸集合の幅は0。頂点列overloadへの非凸入力には
`std::invalid_argument` を送出する。
