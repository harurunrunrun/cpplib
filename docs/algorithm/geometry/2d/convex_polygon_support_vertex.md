---
title: Convex Polygon Support Vertex Aggregator (凸多角形の支持点集約ヘッダ)
documentation_of: ../../../../src/algorithm/geometry/2d/convex_polygon_support_vertex.hpp
---

正規化済み凸多角形入力と頂点列入力の支持点APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するoverload |
| --- | --- |
| `convex_polygon_support_vertex_normalized.hpp` | `convex_polygon_support_vertex(const NormalizedConvexPolygon&, direction)` |
| `convex_polygon_support_vertex_points.hpp` | `convex_polygon_support_vertex(std::vector<Point>, direction)` |

## 集約されるAPI

```cpp
std::size_t convex_polygon_support_vertex(
    const NormalizedConvexPolygon& polygon,
    const Point& direction
);
std::size_t convex_polygon_support_vertex(
    std::vector<Point> polygon,
    const Point& direction
);
```

指定方向との内積を最大化する正規化後の頂点添字を返す。同率なら小さい添字を選ぶ。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。

| overload | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `NormalizedConvexPolygon` | $O(\log N)$ | $O(1)$ |
| `std::vector<Point>` | $O(N)$ | $O(N)$ |

## 注意点

空集合では `CONVEX_POLYGON_NPOS` を返す。零方向では、空でなければ0を返す。
頂点列overloadへの非凸入力には `std::invalid_argument` を送出する。
