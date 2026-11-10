---
title: Sphere Cap Volume (球冠の体積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/sphere/sphere_cap_volume.hpp
---

## API

- `sphere_cap_volume(sphere, height)`: 半径 $r$ の球 `sphere` から切り取った高さ $h$ （`height`）の球冠の体積 $\pi h^2 (r - h/3)$ を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- `sphere` は中心座標と半径が有限、かつ半径が非負でなければならない。また `height` は有限かつ $0 \le h \le 2r$ でなければならない。違反時は `std::invalid_argument` を送出する。
- 積は各因子を仮数部と指数部に分解して計算するため、巨大な半径と微小な高さを含む場合も途中でオーバーフロー・アンダーフローは起きない。最終結果が `long double` で表現できない場合は `std::overflow_error` を送出する。
