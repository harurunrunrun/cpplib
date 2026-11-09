---
title: Farthest Vertices of a Normalized Convex Polygon (正規化凸多角形の最遠頂点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/index_set/polygon/convex_polygon_farthest_vertices_normalized.hpp
---

正規化済みの凸多角形の各頂点に対する最遠頂点を、SMAWK で一括計算する。

## API

```cpp
std::vector<std::size_t> convex_polygon_farthest_vertices(
    const NormalizedConvexPolygon& polygon
);
```

`result[i]` は `polygon.vertices()[i]` から最も遠い頂点の添字である。
行 $i$、列 $j$（$i \lt j \lt i + N$）の成分を頂点 $i$ と頂点 $j \bmod N$ の距離の 2 乗とする暗黙の全単調行列を使い、行列自体は構築しない。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 正規化済み多角形版 | $O(N)$ | $O(N)$ |

## 注意点

- 距離が同じなら、対象の頂点から反時計回りに見て最初に現れる頂点を選ぶ。
- 0 点では空列、1 点では `{0}`、2 点では `{1, 0}` を返す。
- `polygon` は有効な `NormalizedConvexPolygon` でなければならない。
