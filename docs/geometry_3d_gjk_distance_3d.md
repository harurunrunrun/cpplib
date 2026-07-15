---
title: GJK Distance 3D (3次元GJK距離)
documentation_of: ../src/algorithm/geometry/3d/gjk_distance_3d.hpp
---

## API

- `gjk_distance_3d(first, second, tolerance=1e-12L, max_iterations=96)`: 2凸形状間の Euclid 距離を返す。交差時は `0`。

## API別の時間計算量・空間計算量

- $I$ 回反復し、時間 $O(I(V_1+V_2))$、追加領域 $O(1)$。

## 注意点

前提と例外は `gjk_query_3d` と同じ。
