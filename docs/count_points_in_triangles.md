---
title: Count Points in Triangles (三角形内の点数え上げ) [count_points_in_triangle]
documentation_of: ../src/algorithm/geometry/2d/query/count_points_in_triangles.hpp
---

固定された点集合に対し、3頂点が作る三角形の内部にある点数を定数時間で答える。
三角形の辺上にある点は数えない。各頂点を始点とする半直線を偏角順に並べ、
高さ方向の走査とFenwick treeで各有向辺の左右・辺上の点数を前計算する。

## `CountPointsInTriangles`

```cpp
CountPointsInTriangles counter(vertices, query_points);
```

`vertices`から三角形の頂点を選び、`query_points`を数える対象とする。
どちらも`std::vector<std::pair<Coordinate, Coordinate>>`で、`Coordinate`は整数型とする。
外積は128-bit符号付き整数で計算し、中間値がその範囲に収まる必要がある。

## `count`

```cpp
int answer = counter.count(a, b, c);
```

`vertices[a]`, `vertices[b]`, `vertices[c]`の凸包の内部にある対象点数を返す。
3点が同一直線上にある場合、または同じ添字を含む場合は0を返す。
いずれかの添字が`vertex_count()`以上なら`std::out_of_range`を送出する。

## `vertex_count`

```cpp
std::size_t n = counter.vertex_count();
```

構築時に渡した`vertices`の要素数を返す。

## API別の時間計算量・空間計算量

$N=\lvert\mathrm{vertices}\rvert$、$M=\lvert\mathrm{query\_points}\rvert$とする。

| API・操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| constructor | $O(M\log M+N(N+M)\log(N+M))$ | $O(N^2+N+M)$ |
| `count` | $O(1)$ | $O(1)$ |
| `vertex_count` | $O(1)$ | $O(1)$ |

## 注意点

- `Coordinate` は整数型でなければならない。
- 座標差の積と外積は符号付き128-bit整数で計算するため、中間値がその範囲に
  収まる必要がある。浮動小数点数や許容誤差は使用しない。
- 点数は `int` で返すため、`query_points.size()` が `int` の上限を超える場合は
  `length_error` を送出する。
