---
title: Great-Circle Distance (大円距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/point_point/great_circle_distance.hpp
---

## API

- `great_circle_distance(first, second, radius = 1)`: 半径 `radius` の球面上で、2 方向を結ぶ短い方の大円弧の長さを返す。

## API別の時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 方向は有限な非零ベクトル、半径は有限な非負値でなければならない。違反時は `std::invalid_argument` を送出する。
- 距離が `long double` で表現できない場合は `std::overflow_error` を送出する。
- 中心角は `spherical_central_angle` で求めるため、ベクトルの長さは無視される。
