---
title: Sphere Volume Cut by 3D Plane (三次元平面で切られる球の体積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/sphere_plane/sphere_plane_cut_volume.hpp
---

## API

- `sphere_plane_cut_volume(sphere, cutting_plane, side)`: 平面 `cutting_plane` が定める半空間のうち、`side` が $1$ なら法線側、$-1$ なら反対側に含まれる球体部分の体積を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- `sphere_cap_height` で球冠の高さを求め、`sphere_cap_volume` に渡して計算する。平面が球と交わらない場合は $0$ または球全体の体積を返す。
- `sphere` は中心座標と半径が有限、かつ半径が非負でなければならない。`side` は $-1$ または $1$ でなければならず、平面の点・法線は有限、法線は零ベクトルであってはならない。違反時は `std::invalid_argument` を送出する。
- 計算結果が `long double` で表現できない場合は `std::overflow_error` を送出する。
