---
title: Dominant Axis from Point3 (三次元ベクトルの支配軸選択)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/point/dominant_axis_from_point3.hpp
---

三次元ベクトルの成分のうち絶対値が最大の軸を選ぶ。

## API

- `dominant_axis(normal)`: `Point3` で表したベクトルについて、絶対値が最大の成分に対応する `DominantAxis3`（`x`, `y`, `z` のいずれか）を返す。

## API別の時間計算量・空間計算量

- `dominant_axis`: 時間・追加空間ともに $O(1)$。

## 注意点

- 絶対値の最大値が同じ成分が複数あるときは、$x$, $y$, $z$ の順に優先する。
- ベクトルのノルムが `GEOMETRY3D_EPS` 以下（零ベクトルとみなされる）の場合は `std::invalid_argument` を送出する。
