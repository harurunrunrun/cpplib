---
title: 3D Plane-Sphere Intersection (三次元平面と球面の交円)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/plane_sphere/plane_sphere_intersection.hpp
---

三次元平面と球面の交わりを円 `Circle3` として求める。

## API

- `plane_sphere_intersection(plane, sphere)`: 交円を `std::optional<Circle3>` で返す。交わらなければ `std::nullopt`、接する場合は半径 $0$ の円を返す。円の法線は `plane.normal` を正規化したものである。

## API別の時間計算量・空間計算量

時間・追加空間ともに $O(1)$。

## 注意点

- `plane` の点・法線は有限で、法線は非零でなければならない。`Sphere3` は中心・半径が有限かつ半径が非負でなければならない。違反した場合は `std::invalid_argument` を送出する。
- 共通のスケールで正規化して計算するため、中間計算のオーバーフローは避けられる。結果の円が `long double` で表現できない場合は `std::overflow_error` を送出する。
