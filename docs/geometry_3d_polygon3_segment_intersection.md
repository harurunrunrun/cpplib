---
title: Polygon3 Segment Intersection (三次元多角形と線分の交差)
documentation_of: ../src/algorithm/geometry/3d/polygon3_segment_intersection.hpp
---

## API

- `polygon3_segment_intersection(polygon, segment)`: 線分と多角形の全共通区間・孤立点を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として時間 $O(N^2)$、領域 $O(N)$。退化線分の点包含は時間 $O(N)$。

## 注意点

線分の両端を含む。退化線分は点として扱う。
