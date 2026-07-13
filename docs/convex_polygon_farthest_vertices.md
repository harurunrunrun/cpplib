---
title: Convex Polygon Farthest Vertices
documentation_of: ../src/algorithm/geometry/convex_polygon_farthest_vertices.hpp
---

凸多角形の各頂点に対する最遠頂点を一括して求める。

```cpp
#include "src/algorithm/geometry/convex_polygon_farthest_vertices.hpp"

std::vector<std::size_t> farthest =
    convex_polygon_farthest_vertices(polygon);
```

## API

```cpp
std::vector<std::size_t> convex_polygon_farthest_vertices(
    const NormalizedConvexPolygon& polygon
);

std::vector<std::size_t> convex_polygon_farthest_vertices(
    std::vector<Point> polygon
);
```

返り値の長さは正規化後の頂点数であり、`result[i]` は `vertices()[i]` から
最も遠い頂点の添字である。同距離の頂点が複数ある場合は、`i` から反時計回りに
最初に現れるものを返す。

距離列は一般の凸多角形では unimodal とは限らない。実装は距離行列を
$N\times(2N-1)$ の暗黙 totally monotone matrix として扱い、SMAWK で各行最大を
線形時間で求める。行列自体は構築しない。

## 退化入力

- 0点: 空列。
- 1点: `{0}`。
- 2点または全点 collinear: `{1, 0}`。

## 計算量

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| 正規化済み overload | $O(N)$ | $O(N)$ |
| `vector<Point>` overload | $O(N)$ | $O(N)$ |
