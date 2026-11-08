---
title: Weighted Points in Triangles (三角形内の符号付き点重み和)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/weighted_points_in_triangles.hpp
---

候補頂点集合と重み付き点集合を前処理し、候補頂点 3 つからなる三角形の内部にある点の重みの和に答える。

## API

```cpp
template<std::integral Coordinate>
class WeightedPointsInTriangles;
```

- `WeightedPointsInTriangles<Coordinate>(vertices, points)`: 候補頂点と重み付き点集合を前処理する。`vertices` は `std::vector<std::pair<Coordinate, Coordinate>>`、`points` は `std::vector<std::tuple<Coordinate, Coordinate, std::int64_t>>`（座標 $x, y$ と重み）である。
- `weighted_sum(a, b, c)`: 候補頂点 $a, b, c$ を頂点とする三角形の、**境界を除く内部** にある点の重みの和を `std::int64_t` で返す。
- `vertex_count()`: 候補頂点数を返す。

負の重みや、同一座標の重複点を許す。

## 時間計算量

候補頂点数を $N$、点数を $M$ とする。
各頂点のまわりで偏角順の掃引を行い、Fenwick tree に符号付き重みを加算して前計算する。

- 構築: 時間 $O(M \log (M+1) + N(N+M) \log (N+M+1))$、空間 $O(N^2 + M)$。
- `weighted_sum`: 時間 $O(1)$、追加空間 $O(1)$。

## 注意点

- 重みは任意の `std::int64_t` でよい。計算量は重みの種類数には依存しない。途中の集計には `__int128` を使う。
- 頂点番号が範囲外なら `std::out_of_range`、結果が `std::int64_t` に収まらなければ `std::overflow_error` を送出する。
- 点数が `std::numeric_limits<int>::max()` を超えると `std::length_error` を送出する。
- 座標の差の積を含む外積が `__int128` に収まる必要がある。
- 退化した（面積 0 の）三角形に対する和は 0 である。
