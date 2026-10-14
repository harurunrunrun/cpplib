---
title: Weighted Points in Triangles (三角形内の符号付き点重み和)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/weighted_points_in_triangles.hpp
---

`WeightedPointsInTriangles<Coordinate>(vertices,points)` は候補頂点と重み付き点集合を前処理する。
`vertices` は座標対、`points` は `(x,y,int64_t weight)` の配列。
`weighted_sum(a,b,c)` は候補頂点 a,b,c の三角形の**境界を除く内部**にある点の重み和を返す。
負の重み・同一座標の重複点を許す。`vertex_count()` は候補頂点数。

## 時間計算量

候補頂点数 $N$、点数 $M$ とする。
各頂点からの角度 sweep の Fenwick tree に符号付き重みを加算する。
構築は $O(M\log(M+1)+N(N+M)\log(N+M+1))$ 時間、
$O(N^2+N+M)$ 空間。一照会 $O(1)$ 時間、追加空間 $O(1)$。

## 注意点

重みは任意の `int64_t`。種類数には依存しない。途中の集計には `__int128` を使う。
頂点番号の範囲外は `out_of_range`、結果が `int64_t` に収まらなければ `overflow_error`。
座標の差の積を含む外積が GCC 13 の `__int128` に収まること。退化した三角形の和は0。
