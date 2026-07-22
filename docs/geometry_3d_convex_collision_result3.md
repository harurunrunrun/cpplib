---
title: Convex Collision Result 3D (3次元凸形状衝突結果)
documentation_of: ../src/algorithm/geometry/3d/query/convex_collision_result3.hpp
---

## API

- `ConvexCollisionResult3::intersects`: 接触または交差するとき真。
- `distance`: 非交差時の距離。交差時は `0`。
- `penetration_depth`: 交差時の最小並進深さ。非交差時は `0`。
- `normal`: 第1形状から第2形状を向く単位法線。
- `point_on_first`, `point_on_second`: 最近点または接触代表点。
- `gjk_iterations`, `epa_iterations`, `converged`: 各反復回数と全体の収束状態。

## API別の時間計算量・空間計算量

- 各構造体の構築・参照は $O(1)$。

## 注意点

接触点と法線は対称形や面接触では一意でない。

非交差時および3次元内部を持つ形状同士の交差では `normal` は有限な単位ベクトル。退化形状との交差では貫通方向を定義せず `normal=(0,0,0)`、`penetration_depth=0` とする。
