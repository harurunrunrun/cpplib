---
title: Farthest Vertices of a Normalized Convex Polygon (正規化凸多角形の最遠頂点)
documentation_of: ../../../../src/algorithm/geometry/2d/convex_polygon_farthest_vertices_normalized.hpp
---

正規化済み凸多角形の各頂点に対する最遠頂点をSMAWKで一括計算する。

## API

```cpp
std::vector<std::size_t> convex_polygon_farthest_vertices(
    const NormalizedConvexPolygon& polygon
);
```

`result[i]` は `polygon.vertices()[i]` から最も遠い頂点の添字である。
暗黙のtotally monotoneな距離行列を使い、行列自体は構築しない。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 正規化済みoverload | $O(N)$ | $O(N)$ |

## 注意点

- 同距離なら対象頂点から反時計回りに最初に現れる頂点を選ぶ。
- 0点では空列、1点では `{0}`、2点では `{1, 0}` を返す。
- `polygon` は有効な `NormalizedConvexPolygon` でなければならない。
