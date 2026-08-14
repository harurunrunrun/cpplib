---
title: General Polygon Intersection Predicate (一般多角形の交差判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/point_set_point_set/intersect_polygons.hpp
---

## API

- `intersect_polygons(first, second)`: 凸性を仮定しない2つの単純多角形の閉領域が共有点を持つかを返す。

## API別の時間計算量・空間計算量

頂点数を $N,M$ とする。AABB階層構築は $O(N\log N+M\log M)$、辺候補探索の最悪時間計算量は $O(NM)$、追加空間計算量は $O(N+M)$。

## 注意点

空入力は `false`。有限座標の単純多角形を頂点の境界順で与える。接触と包含も交差に含め、自己交差多角形は対象外とする。
