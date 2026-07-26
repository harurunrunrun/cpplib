---
title: Minimum Enclosing Rectangle for Normalized Polygon (正規化済み凸多角形の最小外接長方形)
documentation_of: ../../../../src/algorithm/geometry/2d/minimum_enclosing_rectangle_normalized.hpp
---

正規化済み凸多角形を含む面積最小の長方形を求める。

```cpp
#include "src/algorithm/geometry/2d/minimum_enclosing_rectangle_normalized.hpp"
```

## API

```cpp
MinimumEnclosingRectangleResult minimum_enclosing_rectangle(
    const NormalizedConvexPolygon& polygon
);
```

各辺方向に対する4本の支持直線を単調に更新し、面積が最小の候補を返す。

## 退化入力

- 0点: `valid == false` で、全数値が0の結果を返す。
- 1点: 4頂点がその点と等しく、幅、高さ、面積が0の有効な結果を返す。
- 2点または全点が一直線上: 両端を辺とする高さ0の有効な結果を返す。

## 時間計算量

正規化後の頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `minimum_enclosing_rectangle(polygon)` | $O(N)$ | $O(1)$ |

4本の支持点は多角形を高々定数回巡回する。

## 注意点

入力は `NormalizedConvexPolygon` の契約を満たす必要がある。複数の最小長方形が
存在する場合はそのうち1つを返す。座標と内積、面積は有限な `long double` の
範囲に収まる必要がある。

## 使用例

```cpp
const std::vector<Point> vertices{
    {0, 0}, {4, 0}, {4, 2}, {0, 2},
};
NormalizedConvexPolygon polygon(vertices);
const auto result = minimum_enclosing_rectangle(polygon);
```
