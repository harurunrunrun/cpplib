---
title: 3D Barycentric Coordinate Detail (三次元重心座標内部処理)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/point_point_point/barycentric_coordinates_detail.hpp
---

## API

名前空間 `geometry3d_barycentric_detail` に次の関数を置く。

- `edge_scale(points)`: 先頭の頂点から他の各頂点への座標差の正規化スケールのうち最大のものを返す（すべての差が零なら 0）。
- `normalized_difference(left, right, scale)`: 座標差 `left - right` を指定したスケールで正規化する。
- `vector_scale(...)`: 2 本または 3 本のベクトルの座標の最大絶対値を返す。

## API別の時間計算量・空間計算量

- 全 API: 時間・追加領域ともに $O(1)$。

## 注意点

公開 API ではない。入力点は有限でなければならず、`edge_scale` は非有限な点に `std::invalid_argument` を送出する。
