---
title: 2D Point in Convex Polygon (二次元凸多角形包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/point_set_point/contains_convex.hpp
---

## API

- `int contains_convex(const std::vector<Point>& polygon, const Point& point)`: 凸多角形を正規化したうえで、外部なら 0、境界上なら 1、内部なら 2 を返す。内部では `ConvexPolygonQuery(polygon).contains(point)` を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(N)$。正規化後の判定自体は $O(\log N)$ である。

## 注意点

時計回りの入力、重複頂点、辺の途中にある共線な頂点、点・線分への退化に対応する。入力は凸でなければならない。
同じ多角形に多数のクエリを投げる場合は、`ConvexPolygonQuery` を一度だけ構築して `contains` を呼ぶと、1 回あたり $O(\log N)$ になる。
