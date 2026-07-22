---
title: Upper Convex Hull (上側凸包)
documentation_of: ../src/algorithm/geometry/2d/point_collection/upper_convex_hull.hpp
---

## API

- `upper_convex_hull(points, keep_collinear=false)`: 凸包の上側chainを作る。

## 引数

`points` は点集合、`keep_collinear` はchain上の中間共線点を残す指定である。

## 戻り値

左端から右端へ並ぶ上側chainを返す。

## API別の時間計算量・空間計算量

点数を $N$ として時間 $O(N\log N)$、領域 $O(N)$。

## 注意点

重複点は除く。全点共線で `keep_collinear=false` なら両端だけを返す。
