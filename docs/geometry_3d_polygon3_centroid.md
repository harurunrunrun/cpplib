---
title: Polygon3 Centroid (三次元多角形の重心)
documentation_of: ../src/algorithm/geometry/3d/polygon3_centroid.hpp
---

## API

- `polygon3_centroid(polygon)`: 面積に関する多角形重心を返す。

## API別の時間計算量・空間計算量

- 頂点数を$N$として時間・追加領域$O(N)$。

## 注意点

面積0の多角形には`std::invalid_argument`を送出する。
