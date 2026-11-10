---
title: Convex Collision Result 3D (3次元凸形状衝突結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/convex_collision_result3.hpp
---

## API

- `ConvexCollisionResult3::intersects`: 2 つの形状が接触または交差するとき `true`。
- `distance`: 交差しないときの距離。交差するときは $0$。
- `penetration_depth`: 交差するときの最小並進深さ（めり込みを解消するのに必要な最小の移動量）。交差しないときは $0$。
- `normal`: 第 1 の形状から第 2 の形状へ向かう単位法線。
- `point_on_first`, `point_on_second`: 各形状上の最近点、または接触の代表点。
- `gjk_iterations`, `epa_iterations`: GJK・EPA それぞれの反復回数。
- `converged`: 全体が収束したかどうか。

## API別の時間計算量・空間計算量

- 構築・各メンバの参照: 時間・追加領域 $O(1)$。

## 注意点

- 対称な形状や面同士の接触では、接触点と法線は一意に定まらない。
- 交差しない場合と、三次元の内部を持つ形状同士が交差する場合は、`normal` は有限な単位ベクトルである。退化した形状との交差では貫通方向を定義せず、`normal` を $(0, 0, 0)$、`penetration_depth` を $0$ とする。
