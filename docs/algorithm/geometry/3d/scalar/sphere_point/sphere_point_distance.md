---
title: Point to 3D Sphere Distance (点と三次元球面の距離)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/sphere_point/sphere_point_distance.hpp
---

## API

- `distance(sphere, point)`: 点 `point` と球面 `sphere` の最短距離 $\lvert \lVert p - c \rVert - r \rvert$ を返す。

## API別の時間計算量・空間計算量

- 時間・追加領域 $O(1)$。

## 注意点

- 球体ではなく球面への距離である。点が球の内部にあっても $0$ にはならない。
- `sphere` は中心座標と半径が有限、かつ半径が非負でなければならず、`point` も有限でなければならない。違反時は `std::invalid_argument` を送出する。
- 座標差と半径を共通スケールで正規化して計算するため、中間計算でオーバーフローは起きない。結果が `long double` で表現できない場合は `std::overflow_error` を送出する。
