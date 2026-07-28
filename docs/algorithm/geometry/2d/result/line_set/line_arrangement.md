---
title: Line Arrangement (直線アレンジメント)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/line_set/line_arrangement.hpp
---

無限直線集合が作る平面埋め込みの1-skeletonを構築する。

## API

`line_arrangement(lines)` は交点 `vertices`、隣接交点間の `bounded_edges`、両端の `rays`、
交点を持たない `unbounded_lines` を返す。各要素は元の直線番号を持つ。

## 時間計算量

直線数を $N$、交点record数を $K=O(N^2)$ とすると
$O(N^2+K\log K)$ 時間、$O(K+N)$ 空間。

## 注意点

face列挙は行わず、完全な1-skeletonを返す。平行・一致する直線対は交点を作らない。
退化直線と非有限座標は拒否する。
