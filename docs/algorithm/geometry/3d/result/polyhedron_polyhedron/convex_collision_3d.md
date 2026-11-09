---
title: Convex Collision Query 3D (3次元凸形状衝突問い合わせ)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron_polyhedron/convex_collision_3d.hpp
---

2 つの凸多面体の衝突判定を行い、交差しなければ距離と最近点を、交差すれば貫通深度と接触法線を求める。

## API

- `convex_collision_3d(first, second, tolerance = 1e-10L, gjk_max_iterations = 96, epa_max_iterations = 128)`: 交差しない場合は GJK による距離・最近点を、交差する場合は EPA による貫通深度・接触法線を、同じ結果型 `ConvexCollisionResult3` で返す。結果は `intersects`, `distance`, `penetration_depth`, `normal`, `point_on_first`, `point_on_second`, `gjk_iterations`, `epa_iterations`, `converged` を持つ。

## API別の時間計算量・空間計算量

- 頂点数を $V_1, V_2$、GJK の反復回数を $I_G$、EPA の反復回数を $I_E$、EPA の最大面数を $F$ とすると、時間 $O(I_G(V_1+V_2) + I_E(V_1+V_2+F))$、追加空間 $O(V_1+V_2+I_E+F)$（入力を正規化したコピーを含む）。

## 注意点

- 有限な頂点を持つ空でない凸形状を前提とする。空の形状、非有限な頂点、非正・非有限の `tolerance`、反復上限 $0$ には `std::invalid_argument` を送出する。
- `normal` は第 1 形状から第 2 形状へ向かう向きで、第 1 形状を `-normal * penetration_depth` だけ動かすと分離できる。
- GJK と EPA は共通の平行移動を除去し、全頂点の広がりで無次元化してから計算する。そのため `tolerance` は相対値であり、一様な拡大縮小や共通の平行移動に依存しない。反復上限に達した場合は `converged = false` となる。
- 距離・貫通深度・最近点・接触点は入力の座標系に戻して返す。真の値が `long double` で表現できない場合は `std::overflow_error` を送出する。定義される場合の `normal` は有限な単位ベクトルである。
- 点・線分・平面など、少なくとも一方が `affine_dimension != 3` の形状で交差する場合は貫通深度を定義せず、`penetration_depth = 0`、`normal = (0, 0, 0)`、`epa_iterations = 0` とし、GJK から復元した有限な接触代表点を返す。交差しない場合は、次元によらず通常の距離と単位法線を返す。
