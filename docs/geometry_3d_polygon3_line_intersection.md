---
title: Polygon3 Line Intersection (三次元多角形と直線の交差)
documentation_of: ../src/algorithm/geometry/3d/polygon3_line_intersection.hpp
---

## API

- `Polygon3LinearIntersection3`: 孤立交点 `points` と互いに内部が素な共通区間 `segments` を保持し、`empty()` で空判定する。
- `polygon3_line_intersection(polygon, line)`: 一般の単純平面多角形と直線の共通部分を返す。非凸形状の複数区間にも対応する。

## API別の時間計算量・空間計算量

- 頂点数を $N$、境界event数を $K$ として時間 $O(NK+K\log K)$（$K\le 2N$）、領域 $O(K)$。
- `empty()` は時間・領域ともに $O(1)$。

## 注意点

3頂点未満、非共面、退化直線を拒否する。境界を共通部分に含める。
