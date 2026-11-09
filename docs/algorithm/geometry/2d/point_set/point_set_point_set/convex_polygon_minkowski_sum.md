---
title: Convex Polygon Minkowski Sum (凸多角形のミンコフスキー和)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point_set/point_set_point_set/convex_polygon_minkowski_sum.hpp
---

## API

- `std::vector<Point> convex_polygon_minkowski_sum(first, second)`: 2 つの閉凸集合のミンコフスキー和を、`normalize_convex_polygon` で正規化した頂点列として返す。

## API別の時間計算量・空間計算量

- `convex_polygon_minkowski_sum`: 頂点数を $N, M$ として、正規化を含めて時間・追加領域 $O(N + M)$。

## 注意点

- 各入力は弱凸な周回列であり、点や線分に退化していてもよい。
- 空集合との和は空列になる。
- 無効な周回列には `std::invalid_argument` を送出する。
