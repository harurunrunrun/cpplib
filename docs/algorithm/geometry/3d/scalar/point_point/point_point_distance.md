---
title: Point-Point Distance (点と点の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/point_point/point_point_distance.hpp
---

## API

- `distance(left, right)`: 2 点間のユークリッド距離を返す。

## API別の時間計算量・空間計算量

- `distance`: 時間・追加領域ともに $O(1)$。

## 注意点

- 座標は有限値でなければならず、違反時は `std::invalid_argument` を送出する。
- 差を正規化してから長さを求めるため中間でオーバーフローしにくいが、結果を `long double` で表現できない場合は `std::overflow_error` を送出する。
