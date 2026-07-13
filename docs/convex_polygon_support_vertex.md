---
title: Convex Polygon Support Vertex (凸多角形の支持点)
documentation_of: ../src/algorithm/geometry/convex_polygon_support_vertex.hpp
---

指定方向に最も遠い凸多角形の頂点。

```cpp
#include "src/algorithm/geometry/convex_polygon_support_vertex.hpp"

std::size_t index = convex_polygon_support_vertex(polygon, direction);
```

## API

```cpp
std::size_t convex_polygon_support_vertex(
    const NormalizedConvexPolygon& polygon,
    const Point& direction
);

std::size_t convex_polygon_support_vertex(
    std::vector<Point> polygon,
    const Point& direction
);
```

`dot(vertices()[index], direction)` を最大にする添字を返す。support line が辺と
重なって2頂点が最大になる場合は、小さい添字を返す。

正規化済み overload は、構築時に保存した単調な辺角度列を二分探索する。

## 退化入力

- 0点: `CONVEX_POLYGON_NPOS`。
- 1点: 0。
- 2点または全点 collinear: 内積が大きい端点。同値なら0。
- `direction == Point{0, 0}`: 全頂点が同値なので、空でなければ0。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 正規化済み overload | $O(\log N)$ | $O(1)$ |
| `vector<Point>` overload | $O(N)$ | $O(N)$ |

## 注意点

座標と中間演算は有限な `long double` の範囲に収まる必要がある。境界・退化判定には各APIで明記した許容誤差を用いる。
