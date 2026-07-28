---
title: 3D Centroid Detail (三次元重心内部処理)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point/centroid_detail.hpp
---

## API

- `geometry3d_centroid_detail::centroid(points)`: 固定長点配列の算術平均を求める内部関数。
- `geometry3d_centroid_detail::average_coordinate(coordinates)`: 一座標の平均をscale正規化して求める内部関数。

## API別の時間計算量・空間計算量

- 要素数を $N$ として、各APIは時間・追加領域 $O(N)$。

## 注意点

公開APIではない。非有限座標には `std::invalid_argument` を送出する。
