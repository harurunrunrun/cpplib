---
title: Pick Theorem (ピックの定理)
documentation_of: ../src/algorithm/geometry/2d/pick_theorem.hpp
---

## API

`PickTheoremResult pick_theorem(polygon)`

## 引数

3頂点以上の単純格子多角形を渡す。

## 戻り値

`boundary_points` と `interior_points` を返す。

## API別の時間計算量・空間計算量

- `pick_theorem`: 頂点数 $N$ として時間 $O(N\log C)$、追加領域 $O(1)$。

## 注意点

例外条件は `lattice_polygon_interior_points` と同じ。
