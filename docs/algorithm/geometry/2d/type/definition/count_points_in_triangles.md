---
title: Count Points in Triangles (三角形内の点数え上げ) [count_points_in_triangle]
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/count_points_in_triangles.hpp
---

固定された点集合に対し、頂点候補から選んだ 3 点が作る三角形の内部にある点の個数（または重みの和）を定数時間で答える。
三角形の辺上にある点は数えない。各頂点について、それより上にある点を偏角順に並べ、
高さ方向の走査と Fenwick tree で各有向辺の左側・辺上にある点の個数を前計算する。

## コンストラクタ

```cpp
template<std::integral Coordinate, class Weight = int>
class CountPointsInTriangles;

CountPointsInTriangles<Coordinate, Weight> counter(vertices, query_points, weights = {});
```

`vertices` から三角形の頂点を選び、`query_points` を数える対象とする。
どちらも `std::vector<std::pair<Coordinate, Coordinate>>` で、`Coordinate` は整数型とする。

`weights`（`std::vector<Weight>`）を渡すと、同じ前処理・クエリ計算量で、点の個数の代わりに内部の点の重みの和を返す。
`Weight` のデフォルトは `int` で、`weights` を省略する（空にする）と各点の重みは $1$ となる。
空でない `weights` は `query_points` と同じ長さでなければならず、異なる場合は `std::invalid_argument` を送出する。
加減算の中間値は `Weight` に収まる必要がある。
符号付き 64 bit の点の重みを安全に集計したい場合は `WeightedPointsInTriangles` を利用できる。

## `count`

```cpp
Weight answer = counter.count(a, b, c);
```

`vertices[a]`, `vertices[b]`, `vertices[c]` を頂点とする三角形の内部にある対象点の個数（重みの和）を返す。
3 点が同一直線上にある場合や、同じ添字を含む場合は $0$ を返す。
いずれかの添字が `vertex_count()` 以上なら `std::out_of_range` を送出する。

## `vertex_count`

```cpp
std::size_t n = counter.vertex_count();
```

構築時に渡した `vertices` の要素数を返す。

## 計算量

$N = \lvert \mathrm{vertices} \rvert$、$M$ を `query_points` の要素数とする。

| API・操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| コンストラクタ | $O(M \log M + N(N+M) \log (N+M))$ | $O(N^2 + M)$ |
| `count` | $O(1)$ | $O(1)$ |
| `vertex_count` | $O(1)$ | $O(1)$ |

## 注意点

- `Coordinate` は整数型でなければならない。
- 座標差の積と外積は符号付き 128 bit 整数で計算するため、中間値がその範囲に
  収まる必要がある。浮動小数点数や許容誤差は使わない。
- `query_points.size()` が `int` の最大値を超える場合は `std::length_error` を送出する（`Weight` の型によらない）。
