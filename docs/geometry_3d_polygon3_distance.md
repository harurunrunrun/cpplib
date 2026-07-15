---
title: Polygon3 Point Distance (三次元多角形と点の距離)
documentation_of: ../src/algorithm/geometry/3d/polygon3_distance.hpp
---

## API

- `polygon3_distance(polygon, point)`: 閉多角形領域と点の最短距離を返す。

## API別の時間計算量・空間計算量

- 頂点数を$N$として時間$O(N)$、追加領域$O(N)$。

## 注意点

単純な共面多角形を与える。面積0の多角形には`std::invalid_argument`を送出する。
