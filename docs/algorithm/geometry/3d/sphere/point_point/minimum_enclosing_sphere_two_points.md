---
title: Minimum Enclosing Sphere of Two 3D Points (三次元二点の最小包含球)
documentation_of: ../../../../../../src/algorithm/geometry/3d/sphere/point_point/minimum_enclosing_sphere_two_points.hpp
---

2 点を含む半径最小の閉球を定数時間で構築する。

## API

- `minimum_enclosing_sphere(first, second)`: 2 点の中点を中心、2 点間の距離の半分を半径とする `Sphere3` を返す。2 点が一致する場合にも対応する。

## API別の時間計算量・空間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 座標は有限でなければならず、違反時は `std::invalid_argument` を送出する。
- 点の差は局所的なスケールで正規化して計算するため、座標の直接の減算によるオーバーフローは起きない。結果を `long double` で表現できない場合は `std::overflow_error` を送出する。
