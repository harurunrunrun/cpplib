---
title: Sphere Volume Cut by 3D Plane (三次元平面で切られる球の体積)
documentation_of: ../src/algorithm/geometry/3d/scalar/sphere_plane_cut_volume.hpp
---

## API

- `sphere_plane_cut_volume(sphere, cutting_plane, side)`: 指定した平面半空間に含まれる球体部分の体積を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

計算結果が long double で表現できない場合は std::overflow_error を送出する。交差計算は共通移動量を保つ正規化で中間 overflow を避ける。
Sphere3 は中心・半径が有限かつ半径が非負でなければならず、違反時は std::invalid_argument を送出する。

`side` が1なら法線側、-1なら反対側を選ぶ。負半径、零法線、不正な `side` には `std::invalid_argument` を送出する。
