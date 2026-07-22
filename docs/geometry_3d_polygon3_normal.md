---
title: Polygon3 Normal (三次元多角形の法線)
documentation_of: ../src/algorithm/geometry/3d/point/polygon3_normal.hpp
---

## API

- `polygon3_normal(polygon)`: 頂点順に対応する単位法線を返す。

## API別の時間計算量・空間計算量

- 頂点数を$N$として時間$O(N)$、追加領域$O(1)$。

## 注意点

面積0の多角形には`std::invalid_argument`を送出する。
