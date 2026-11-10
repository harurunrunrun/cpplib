---
title: Triangle-Point Distance (三角形と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/triangle_point/triangle_point_distance.hpp
---

## API

- `distance(triangle, point)`: 閉三角形領域（内部を含む）と点 `point` の最短距離を返す。

## API別の時間計算量・空間計算量

- `distance`: 時間・追加領域 $O(1)$。

## 注意点

- 三角形と点の座標は有限値でなければならない。
- 退化した三角形（3 頂点が同一直線上にある三角形）には `std::invalid_argument` を送出する。
- 距離が `long double` で表現できない場合は `std::overflow_error` を送出する。
