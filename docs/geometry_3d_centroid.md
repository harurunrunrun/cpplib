---
title: 3D Centroid (三次元重心)
documentation_of: ../src/algorithm/geometry/3d/centroid.hpp
---

## API

- `centroid(triangle)`: 3頂点の平均を返す。
- `centroid(tetrahedron)`: 4頂点の平均を返す。

## API別の時間計算量・空間計算量

- 各overload: 時間・追加領域 $O(1)$。

## 注意点

退化した図形にも算術平均を返し、退化検査は行わない。
