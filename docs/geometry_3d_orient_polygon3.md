---
title: Orient Polygon3 (三次元多角形の向き統一)
documentation_of: ../src/algorithm/geometry/3d/orient_polygon3.hpp
---

## API

- `orient_polygon3(polygon, normal)`: 先頭頂点を保ち、法線方向から反時計回りになる頂点順を返す。

## API別の時間計算量・空間計算量

- 頂点数を$N$として時間$O(N)$、戻り値を含む領域$O(N)$。

## 注意点

零参照法線または3頂点未満には`std::invalid_argument`を送出する。
