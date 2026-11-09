---
title: 3D Line-Ray Skewness (三次元直線と半直線のねじれ判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/aggregate/all/line_ray_skew.hpp
---

## API

- `skew(line, ray)`: `Line3` と `Ray3` の支持直線同士がねじれの位置にあるなら `true` を返す。
- `skew(ray, line)`: 引数の順序を入れ替えた同じ判定を行う。

## API 別の時間計算量・空間計算量

- 全オーバーロード: 時間・追加空間ともに $O(1)$。

## 注意点

- 半直線の向きや始点による範囲の制限は考慮しない（支持直線同士で判定する）。
- 退化した直線または退化した半直線には `std::invalid_argument` を送出する。
