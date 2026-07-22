---
title: 2D Point in Polygon (二次元多角形包含判定) [CGL_3_C]
documentation_of: ../src/algorithm/geometry/2d/predicate/contains.hpp
---

## API

- `contains(polygon, point)`: 外部なら0、境界上なら1、内部なら2を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(1)$。

## 注意点

単純多角形を頂点順に与える。時計回り・反時計回りのどちらでもよい。
境界は `on_segment` と同じ絶対距離 `GEOMETRY_EPS` で判定し、ray crossingのy方向比較はhalf-open規則を厳密比較で行う。
