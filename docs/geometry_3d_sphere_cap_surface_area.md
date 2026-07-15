---
title: Sphere Cap Surface Area (球冠の曲面積)
documentation_of: ../src/algorithm/geometry/3d/sphere_cap_surface_area.hpp
---

## API

- `sphere_cap_surface_area(sphere, height)`: 高さ `height` の球冠の曲面積 $2\pi rh$ を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点
積は各因子を仮数部と指数部へ分解して計算するため、途中の overflow / underflow を避ける。最終結果が `long double` で表現できない場合は `std::overflow_error` を送出する。
Sphere3 は中心・半径が有限かつ半径が非負でなければならず、違反時は std::invalid_argument を送出する。

半径は非負、`height` は $[0,2r]$ でなければならず、違反時は `std::invalid_argument` を送出する。
