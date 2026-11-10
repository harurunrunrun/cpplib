---
title: 3D Sphere Cap Height (三次元球冠の高さ)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/sphere_plane/sphere_cap_height.hpp
---

## API

- `sphere_cap_height(sphere, cutting_plane, side)`: 平面 `cutting_plane` で球 `sphere` を切ったとき、`side` が $1$ なら平面の法線側、$-1$ なら反対側にある部分を球冠とみなし、その高さを返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 高さは $[0, 2r]$ にクランプされるため、球と交わらない平面にも対応する（選んだ側に球が無ければ $0$、球全体が含まれれば $2r$）。
- `sphere` は中心座標と半径が有限、かつ半径が非負でなければならない。`side` は $-1$ または $1$ でなければならず、平面の点・法線は有限、法線は零ベクトルであってはならない。違反時は `std::invalid_argument` を送出する。
- 中心と平面上の点の差を共通スケールで正規化して計算するため、中間計算でオーバーフローは起きない。結果が `long double` で表現できない場合は `std::overflow_error` を送出する。
