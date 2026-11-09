---
title: Convex Polygon Minkowski Difference (凸多角形のミンコフスキー差)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set_point_set/convex_polygon_minkowski_difference.hpp
---

## API

- `std::vector<Point> convex_polygon_minkowski_difference(first, second)`: `first` を $A$、`second` を $B$ として、$\lbrace a - b \mid a \in A, b \in B \rbrace$ を返す。

## 引数

`first`, `second` は弱凸な周回列（`std::vector<Point>`、値渡し）である。

## 戻り値

差集合を、`normalize_convex_polygon` で正規化した点・線分・反時計回りの凸多角形として返す。

## API別の時間計算量・空間計算量

- `convex_polygon_minkowski_difference`: 頂点数を $N, M$ として、時間・領域 $O(N + M)$。

## 注意点

- `second` の各点の符号を反転し、`convex_polygon_minkowski_sum` に帰着する。
- どちらかが空なら空列を返す。凸でない入力には `std::invalid_argument` を送出する。
