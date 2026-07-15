---
title: GJK Query 3D (3次元GJK問い合わせ)
documentation_of: ../src/algorithm/geometry/3d/gjk_query_3d.hpp
---

## API

- `gjk_query_3d(first, second, tolerance=1e-12L, max_iterations=96)`: support mapping と縮約単体を用い、交差、距離、両形状上の最近点をまとめて返す。

## API別の時間計算量・空間計算量

- 頂点数を $V_1,V_2$、実反復数を $I$ として時間 $O(I(V_1+V_2))$、追加領域 $O(1)$（返却単体も高々4点）。

## 注意点

有限頂点を持つ空でない凸形状が前提。非正または非有限の許容誤差、反復上限0は `std::invalid_argument`。反復上限到達時は `converged=false`。
