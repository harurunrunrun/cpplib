---
title: Minimum Enclosing Rectangle from Points (点列からの最小外接長方形)
documentation_of: ../../../../src/algorithm/geometry/2d/minimum_enclosing_rectangle_points.hpp
---

点列を正規化してから、その凸集合を含む面積最小の長方形を求める。

```cpp
#include "src/algorithm/geometry/2d/minimum_enclosing_rectangle_points.hpp"
```

## API

```cpp
MinimumEnclosingRectangleResult minimum_enclosing_rectangle(
    std::vector<Point> polygon
);
```

`polygon` は値渡しされ、向き、重複頂点、共線頂点を正規化する。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `minimum_enclosing_rectangle(polygon)` | $O(N)$ | $O(N)$ |

正規化とrotating calipersはいずれも線形時間である。

## 注意点

点列は凸多角形の境界を巡回する順に並んでいる必要がある。非凸または自己交差する
点列は `std::invalid_argument` の対象となる。複数の最小長方形が存在する場合は
そのうち1つを返す。座標と中間演算は有限な `long double` の範囲に収まる必要がある。

## 使用例

```cpp
std::vector<Point> polygon{
    {0, 0}, {4, 0}, {4, 2}, {0, 2},
};
const auto result = minimum_enclosing_rectangle(std::move(polygon));
```
