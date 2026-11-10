---
title: 3D Centroid Detail (三次元重心内部処理)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point/centroid_detail.hpp
---

## API

- `geometry3d_centroid_detail::centroid(points)`: 固定長の点配列 `std::array<Point3, Size>` の算術平均を求める内部関数。
- `geometry3d_centroid_detail::average_coordinate(coordinates)`: 1 つの座標成分の平均を、最大絶対値で正規化してから求める内部関数。

## API別の時間計算量・空間計算量

- 要素数を $N$ として、各 API とも時間・追加領域 $O(N)$。

## 注意点

公開 API ではない。`centroid` は非有限な座標に対して `std::invalid_argument` を送出する。
