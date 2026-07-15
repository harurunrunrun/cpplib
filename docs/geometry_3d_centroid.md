---
title: 3D Centroid (三次元重心)
documentation_of: ../src/algorithm/geometry/3d/centroid.hpp
---

## API

- `centroid(triangle)`: 3頂点の平均を返す。
- `centroid(tetrahedron)`: 4頂点の平均を返す。

## API別の時間計算量・空間計算量

- `centroid(triangle)`: 時間・追加領域 $O(1)$。
- `centroid(tetrahedron)`: 時間・追加領域 $O(1)$。

## 注意点

- 頂点はすべて有限でなければならず、違反時は `std::invalid_argument` を
  送出する。
- 各座標をその最大絶対値で正規化して平均するため、3点・4点を先に加算する
  実装で発生する中間overflowを避ける。
- 退化した図形にも算術平均を返し、退化検査は行わない。
