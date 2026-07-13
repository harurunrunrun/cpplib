---
title: 2D Convexity Test (二次元凸性判定)
documentation_of: ../src/algorithm/geometry/2d/is_convex.hpp
---

## API

- `is_convex(polygon)`: 頂点順に結んだ多角形が凸か判定する。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、追加領域 $O(1)$。

## 注意点

時計回り・反時計回りのどちらにも対応し、共線な辺を許す。自己交差のない頂点列を前提とする。
