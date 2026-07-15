---
title: Convex Collision Query 3D (3次元凸形状衝突問い合わせ)
documentation_of: ../src/algorithm/geometry/3d/convex_collision_3d.hpp
---

## API

- `convex_collision_3d(first, second, tolerance=1e-10L, gjk_max_iterations=96, epa_max_iterations=128)`: 非交差時は GJK 距離・最近点、交差時は EPA 貫通深度・接触法線を同じ結果型で返す。

## API別の時間計算量・空間計算量

- GJK を $I_G$ 回、EPA を $I_E$ 回、EPA 最大面数を $F$ として時間 $O(I_G(V_1+V_2)+I_E(V_1+V_2+F))$、領域 $O(I_E+F)$。

## 注意点

有限頂点を持つ空でない凸形状が前提。`normal` は第1形状から第2形状向きで、第1形状を `-normal * penetration_depth` 動かすと分離できる。非正・非有限の許容誤差は例外。
