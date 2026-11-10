---
title: Sphere Surface Area (球の表面積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/sphere/sphere_surface_area.hpp
---

## API

- `sphere_surface_area(sphere)`: 半径 $r$ の球の表面積 $4\pi r^2$ を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- `sphere` は中心座標と半径が有限、かつ半径が非負でなければならず、違反時は `std::invalid_argument` を送出する。
- 積は各因子を仮数部と指数部に分解して計算するため、途中でオーバーフローは起きない。最終結果が `long double` で表現できない場合は `std::overflow_error` を送出する。
