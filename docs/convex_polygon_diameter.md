---
title: Convex Polygon Diameter (凸多角形の直径)
documentation_of: ../src/algorithm/geometry/2d/convex_polygon_diameter.hpp
---

凸多角形の最遠点対と直径。

```cpp
#include "src/algorithm/geometry/2d/convex_polygon_diameter.hpp"

ConvexPolygonDiameterResult result = convex_polygon_diameter(polygon);
```

## 返り値

```cpp
struct ConvexPolygonDiameterResult {
    std::size_t first, second;
    long double squared_distance;
    long double distance() const;
};
```

`first`, `second` は最遠点対の添字で、`squared_distance` は距離の二乗である。
複数の最遠点対がある場合はいずれか1組を返す。

## API

```cpp
ConvexPolygonDiameterResult convex_polygon_diameter(
    const NormalizedConvexPolygon& polygon
);

ConvexPolygonDiameterResult convex_polygon_diameter(
    std::vector<Point> polygon
);
```

後者は弱凸入力を正規化してから計算する。添字は正規化後の頂点列に対応する。

## 退化入力

- 0点: `first == second == CONVEX_POLYGON_NPOS`、距離0。
- 1点: `(first, second) == (0, 0)`、距離0。
- 2点または全点 collinear: 正規化後の両端を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 正規化済み overload | $O(N)$ | $O(N)$ |
| `vector<Point>` overload | $O(N)$ | $O(N)$ |
| `ConvexPolygonDiameterResult::distance` | $O(1)$ | $O(1)$ |

最遠頂点列を暗黙 totally monotone matrix と SMAWK で求め、その最大を選ぶ。

## 注意点

座標と中間演算は有限な `long double` の範囲に収まる必要がある。境界・退化判定には各APIで明記した許容誤差を用いる。
