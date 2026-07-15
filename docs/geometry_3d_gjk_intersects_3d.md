---
title: GJK Intersection 3D (3次元GJK交差判定)
documentation_of: ../src/algorithm/geometry/3d/gjk_intersects_3d.hpp
---

## API

- `gjk_intersects_3d(first, second, tolerance=1e-12L, max_iterations=96)`: 2凸形状が接触または交差するかを返す。

## API別の時間計算量・空間計算量

- $I$ 回反復し、時間 $O(I(V_1+V_2))$、追加領域 $O(1)$。

## 注意点

前提と例外は `gjk_query_3d` と同じ。許容誤差以内の接触を交差とする。

許容誤差は共通平行移動を除いた両形状の広がりに対する相対値である。
