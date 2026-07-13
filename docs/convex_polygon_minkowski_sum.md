---
title: Convex Polygon Minkowski Sum (凸多角形のミンコフスキー和)
documentation_of: ../src/algorithm/geometry/advanced/convex_polygon_minkowski_sum.hpp
---

## API

- `convex_polygon_minkowski_sum(first, second)`: 2閉凸集合のMinkowski和を正規化済み頂点列で返す。

## API別の時間計算量・空間計算量

- 頂点数を $N,M$ として、正規化を含め時間・追加領域 $O(N+M)$。

## 注意点

各入力は弱凸な周回列で、点・線分へ退化してもよい。空集合との和は空。無効な周回列には `std::invalid_argument` を送出する。
