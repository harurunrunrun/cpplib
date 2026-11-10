---
title: Point on Convex Hull Aggregator (凸包上の点判定集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/on_convex_hull.hpp
---

前処理済みのクエリ構造を入力とする版と、頂点列を入力とする版の凸包境界判定 API をまとめた後方互換の集約ヘッダである。

## 構成

| leaf header | 提供するオーバーロード |
| --- | --- |
| `on_convex_hull_query.hpp` | `on_convex_hull(const ConvexPolygonQuery&, point)` |
| `on_convex_hull_points.hpp` | `on_convex_hull(const std::vector<Point>&, point)` |

## 集約される API

```cpp
bool on_convex_hull(const ConvexPolygonQuery& polygon, const Point& point);
bool on_convex_hull(const std::vector<Point>& polygon, const Point& point);
```

点が閉凸集合の境界上にあれば `true` を、内部または外部にあれば `false` を返す。

## 計算量

頂点数を $N$ とする。

| オーバーロード | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `ConvexPolygonQuery` | $O(\log N)$ | $O(1)$ |
| `std::vector<Point>` | $O(N)$ | $O(N)$ |

## 注意点

- 点・線分に退化した凸集合では、その点または線分全体を境界として扱う。
- 頂点列の入力は `ConvexPolygonQuery` の前提（凸多角形の周回列）を満たさなければならない。
