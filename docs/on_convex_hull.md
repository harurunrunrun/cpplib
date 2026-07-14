---
title: Point on Convex Hull (凸包上の点判定)
documentation_of: ../src/algorithm/geometry/2d/on_convex_hull.hpp
---

## API

- `on_convex_hull(query, point)`: 前処理済み凸多角形の境界上か判定する。
- `on_convex_hull(polygon, point)`: 頂点列を前処理して境界上か判定する。

## 引数

`query` は `ConvexPolygonQuery`、`polygon` は弱凸な周回列、`point` は判定点である。

## 戻り値

点が凸集合の境界上なら `true`、内部または外部なら `false`。

## API別の時間計算量・空間計算量

- 前処理済みoverload: 時間 $O(\log N)$、追加領域 $O(1)$。
- 頂点列overload: 時間 $O(N)$、領域 $O(N)$。

## 注意点

点・線分へ退化した凸集合では、その点または線分全体を境界として扱う。
