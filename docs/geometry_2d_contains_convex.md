---
title: 2D Point in Convex Polygon (二次元凸多角形包含判定)
documentation_of: ../src/algorithm/geometry/2d/contains_convex.hpp
---

## API

- `contains_convex(polygon, point)`: 凸多角形を正規化して、外部0、境界1、内部2を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(N)$。正規化後の判定自体は $O(\log N)$。

## 注意点

時計回り、重複頂点、中間の共線頂点、点・線分への退化に対応する。入力は凸でなければならない。
