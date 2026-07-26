---
title: PlaneCoordinateSystem3 Type (3次元平面座標系型)
documentation_of: ../../../../src/algorithm/geometry/3d/plane_coordinate_system3.hpp
---

3次元平面上の直交座標系を保持する結果型です。

## API

- `origin`: 平面座標の原点です。
- `first_axis`, `second_axis`: 平面内の互いに直交する単位軸です。
- `normal`: 右手系を構成する平面の単位法線です。

## API別の時間計算量・空間計算量

構築・コピー・各メンバ参照は時間・追加空間ともに $O(1)$ です。

## 注意点

このheaderは型だけを提供します。平面からの構築には `plane_coordinate_system_from_plane.hpp` をincludeしてください。