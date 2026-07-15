---
title: Orient Plane Normal 3D (三次元平面法線の向き統一)
documentation_of: ../src/algorithm/geometry/3d/orient_plane_normal.hpp
---

## API

- `orient_plane_normal(plane, direction)`: 法線と`direction`の内積が非負になる向きで平面を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

零法線または零参照方向には`std::invalid_argument`を送出する。
