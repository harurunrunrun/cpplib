---
title: 3D Linear Closest Detail (三次元線形最近点内部処理)
documentation_of: ../../../../src/algorithm/geometry/3d/linear_closest_detail.hpp
---

## API

- `query(first, second)`: 直線・半直線・線分の組に対する最近点対を内部表現から求める。
- `checked_distance`, `restore`: 正規化座標と元の尺度の間をoverflow検査付きで変換する。

## API別の時間計算量・空間計算量

- 全API: 時間・追加領域 $O(1)$。

## 注意点

公開APIではない。入力点は有限値でなければならない。
