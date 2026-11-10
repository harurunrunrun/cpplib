---
title: SAT Result 3D (三次元分離軸判定結果)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/sat_result3.hpp
---

## API

- `SATResult3::intersects`: 分離軸が存在しないとき `true`。デフォルト値は `false`。
- `SATResult3::separation`: 交差しないとき、検査した軸上での正の隙間の最大値。
- `SATResult3::penetration_depth`: 交差するとき、検査した軸上での重なり幅の最小値。
- `SATResult3::axis`: `separation` または `penetration_depth` を実現する単位軸。
- `SATResult3::tested_axes`: 重複を除いて検査した軸の本数。

## API別の時間計算量・空間計算量

- 構築・各メンバの参照: 時間・追加領域ともに $O(1)$。

## 注意点

- `axis` は第 1 の形状から第 2 の形状へ向くように符号をそろえる。両者の中心が一致する場合は符号が一意に定まらない。
- `separation` と `penetration_depth` は入力座標系での長さである。`axis` は有限な単位ベクトルである。
