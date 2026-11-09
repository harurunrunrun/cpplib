---
title: 2D Convexity Test (二次元凸性判定) [CGL_3_B]
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/point_set/is_convex.hpp
---

## API

- `bool is_convex(const std::vector<Point>& polygon)`: 頂点を順に結んだ多角形が凸か判定する。

## API別の時間計算量・空間計算量

- `is_convex`: 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(1)$。

## 注意点

- 時計回り・反時計回りのどちらにも対応し、共線な辺（角度 $180^\circ$ の頂点）を許す。
- 自己交差のない頂点列を前提とする（辺の方向が複数周するかは検査しない）。
- 各頂点での折れ曲がりは、隣接する 2 辺を単位化した外積に対する許容誤差（角度基準）で判定する。
