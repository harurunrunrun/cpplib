---
title: PlaneCoordinateSystem3 Type (3次元平面座標系型)
documentation_of: ../../../../../../src/algorithm/geometry/3d/type/definition/plane_coordinate_system3.hpp
---

三次元空間内の平面上の直交座標系を保持する結果型。

## API

- `origin`: 平面座標の原点。
- `first_axis`, `second_axis`: 平面内の互いに直交する単位ベクトルの軸。
- `normal`: `first_axis`, `second_axis` とともに右手系をなす、平面の単位法線。

## API別の時間計算量・空間計算量

構築・コピー・各メンバの参照は時間・追加領域ともに $O(1)$。

## 注意点

- このヘッダは型だけを提供する。平面から構築するには `plane_coordinate_system_from_plane.hpp` をインクルードする。
