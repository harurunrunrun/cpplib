---
title: Sphere Intersection Volume (球の共通部分体積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/sphere_sphere/sphere_intersection_volume.hpp
---

## API

- `sphere_intersection_volume(first, second)`: 2 つの閉球の共通部分の体積を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 離れている場合、接する場合（いずれも $0$）、一方が他方に含まれる場合（小さい方の球の体積）、同心・一致の場合に対応する。
- 各球は中心座標と半径が有限、かつ半径が非負でなければならず、違反時は `std::invalid_argument` を送出する。
- 中心座標と半径を共通スケールで正規化して計算し、最後にスケールの 3 乗を掛け戻すため、中間計算でオーバーフローは起きない。結果が `long double` で表現できない場合は `std::overflow_error` を送出する。
