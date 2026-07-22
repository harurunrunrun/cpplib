---
title: Convex Polygon3 Cut (三次元凸多角形の半空間切断)
documentation_of: ../src/algorithm/geometry/3d/shape/convex_polygon3_cut.hpp
---

## API

- `convex_polygon3_cut(polygon, plane)`: `plane`の法線側閉半空間に残る部分を頂点順で返す。

## API別の時間計算量・空間計算量

- 頂点数を$N$として時間・戻り値領域$O(N)$。

## 注意点

凸な共面多角形を与える。3頂点未満または零法線には`std::invalid_argument`を送出する。
