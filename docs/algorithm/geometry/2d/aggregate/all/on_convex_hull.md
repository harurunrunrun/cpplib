---
title: Point on Convex Hull Aggregator (凸包上の点判定集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/on_convex_hull.hpp
---

前処理済みquery入力と頂点列入力の凸包境界判定APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するoverload |
| --- | --- |
| `on_convex_hull_query.hpp` | `on_convex_hull(const ConvexPolygonQuery&, point)` |
| `on_convex_hull_points.hpp` | `on_convex_hull(const std::vector<Point>&, point)` |

## 集約されるAPI

```cpp
bool on_convex_hull(
    const ConvexPolygonQuery& polygon,
    const Point& point
);
bool on_convex_hull(
    const std::vector<Point>& polygon,
    const Point& point
);
```

点が閉凸集合の境界上ならtrue、内部または外部ならfalseを返す。

## API別の時間計算量・空間計算量

頂点数を $N$ とする。

| overload | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `ConvexPolygonQuery` | $O(\log N)$ | $O(1)$ |
| `std::vector<Point>` | $O(N)$ | $O(N)$ |

## 注意点

点・線分へ退化した凸集合では、その点または線分全体を境界として扱う。
頂点列入力は `ConvexPolygonQuery` の前提を満たさなければならない。
