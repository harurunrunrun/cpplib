---
title: Polygon3 Segment Intersection (三次元多角形と線分の交差)
documentation_of: ../src/algorithm/geometry/3d/polygon3_segment_intersection.hpp
---

## API

- `polygon3_segment_intersection(polygon, segment)`: 線分と平面多角形の全共通区間・
  孤立点を `Polygon3LinearIntersection3` で返す。

## API別の時間計算量・空間計算量

頂点数を $N$ として、線分の支持直線が多角形の平面を横切る場合は時間 $O(N)$、
平面上にある場合は最悪時間 $O(N\log N)$。追加領域はいずれも $O(N)$。
退化線分を一点として調べる場合は時間 $O(N)$、追加領域 $O(N)$。
返り値の `empty()` は $O(1)$。

## 注意点

線分の両端を共通部分に含む。退化線分は一点として扱う。
