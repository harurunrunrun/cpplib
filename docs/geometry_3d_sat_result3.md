---
title: SAT Result 3D (3次元分離軸判定結果)
documentation_of: ../src/algorithm/geometry/3d/query/sat_result3.hpp
---

## API

- `SATResult3::intersects`: 分離軸が存在しないとき真。
- `separation`: 非交差時、検査軸上の最大の正の隙間。
- `penetration_depth`: 交差時、検査軸上の最小重なり幅。
- `axis`: `separation` または `penetration_depth` を実現する単位軸。
- `tested_axes`: 重複を除いて検査した軸数。

## API別の時間計算量・空間計算量

- スカラー場の構築・参照は $O(1)$。

## 注意点

`axis` は第1形状から第2形状を向くようそろえる。中心が一致する場合は符号が一意でない。

`separation` と `penetration_depth` は入力座標系の長さ。`axis` は有限な単位ベクトルである。
