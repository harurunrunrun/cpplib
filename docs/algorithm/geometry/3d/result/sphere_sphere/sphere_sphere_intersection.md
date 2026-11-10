---
title: 3D Sphere-Sphere Intersection (三次元球面同士の交円)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/sphere_sphere/sphere_sphere_intersection.hpp
---

2 つの球面の交わりを円 `Circle3` として求める。

## API

- `sphere_sphere_intersection(first, second)`: 交円を `std::optional<Circle3>` で返す。交わらない場合（中心が一致し半径が異なる場合を含む）は `std::nullopt`、接する場合は半径 $0$ の円を返す。円の法線は `first` の中心から `second` の中心へ向かう単位ベクトルである。

## API別の時間計算量・空間計算量

- 時間・追加空間ともに $O(1)$。

## 注意点

- `Sphere3` は中心・半径が有限かつ半径が非負でなければならず、違反した場合は `std::invalid_argument` を送出する。
- 2 つの球面が一致する場合は交円が一意に定まらないため、`std::domain_error` を送出する。
- 共通のスケールで正規化して計算するため、中間計算のオーバーフローは避けられる。結果の円が `long double` で表現できない場合は `std::overflow_error` を送出する。
