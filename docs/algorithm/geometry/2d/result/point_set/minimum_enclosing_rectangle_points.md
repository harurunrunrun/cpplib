---
title: Minimum Enclosing Rectangle from Points (点列からの最小外接長方形)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/minimum_enclosing_rectangle_points.hpp
---

凸多角形の頂点列を正規化してから、それを含む面積最小の長方形を求める。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/result/point_set/minimum_enclosing_rectangle_points.hpp"
```

## API

```cpp
MinimumEnclosingRectangleResult minimum_enclosing_rectangle(
    std::vector<Point> polygon
);
```

`polygon` は値渡しで受け取り、`NormalizedConvexPolygon` によって向き・重複頂点・共線頂点を正規化する。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `minimum_enclosing_rectangle(polygon)` | $O(N)$ | $O(N)$ |

正規化と rotating calipers はいずれも線形時間である。

## 注意点

点列は凸多角形の境界を巡回する順に並んでいる必要がある。非凸な点列や自己交差する点列に対しては `std::invalid_argument` を送出する。最小の長方形が複数存在する場合は、そのうちの 1 つを返す。座標と中間演算の値は有限な `long double` の範囲に収まる必要がある。

## 使用例

```cpp
std::vector<Point> polygon{
    {0, 0}, {4, 0}, {4, 2}, {0, 2},
};
const auto result = minimum_enclosing_rectangle(std::move(polygon));
```
