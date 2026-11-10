---
title: Minimum Enclosing Rectangle for Normalized Polygon (正規化済み凸多角形の最小外接長方形)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/minimum_enclosing_rectangle_normalized.hpp
---

正規化済みの凸多角形を含む、面積最小の長方形を求める。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/result/point_set/minimum_enclosing_rectangle_normalized.hpp"
```

## API

```cpp
MinimumEnclosingRectangleResult minimum_enclosing_rectangle(
    const NormalizedConvexPolygon& polygon
);
```

各辺の方向について 4 本の支持直線を単調に更新し、面積が最小となる候補を返す。返り値は 4 頂点 `corners`、幅 `width`、高さ `height`、面積 `area`、有効フラグ `valid` を持つ。

## 退化入力

- 0 点: `valid == false` で、数値がすべて 0 の結果を返す。
- 1 点: 4 頂点がすべてその点に等しく、幅・高さ・面積が 0 の有効な結果を返す。
- 2 点（全点が一直線上の場合も正規化で 2 点になる）: 両端点を結ぶ線分を辺とする、高さ 0 の有効な結果を返す。

## 時間計算量

正規化後の頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `minimum_enclosing_rectangle(polygon)` | $O(N)$ | $O(1)$ |

4 本の支持点はそれぞれ多角形を高々定数周しかしない。

## 注意点

入力は `NormalizedConvexPolygon` の契約を満たす必要がある。最小の長方形が複数存在する場合は、そのうちの 1 つを返す。座標・内積・面積は有限な `long double` の範囲に収まる必要がある。

## 使用例

```cpp
const std::vector<Point> vertices{
    {0, 0}, {4, 0}, {4, 2}, {0, 2},
};
NormalizedConvexPolygon polygon(vertices);
const auto result = minimum_enclosing_rectangle(polygon);
```
