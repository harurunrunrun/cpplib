---
title: Orient Plane Normal 3D (三次元平面法線の向き統一)
documentation_of: ../../../../../../src/algorithm/geometry/3d/plane/plane_point/orient_plane_normal.hpp
---

## API

- `orient_plane_normal(plane, reference_direction)`: 法線と `reference_direction` の内積が非負になる向きに揃えた平面を返す。内積が負なら法線の符号を反転し、通過点はそのまま保つ。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 法線または参照方向の長さが `GEOMETRY3D_EPS`（$10^{-10}$） 以下なら零ベクトルとみなし、`std::invalid_argument` を送出する（絶対許容誤差による判定）。
- 法線は単位化されない。
