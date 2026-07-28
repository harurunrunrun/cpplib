---
title: Counterclockwise Classification of Three Points (3点の反時計回り分類)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/point_point_point/ccw_points.hpp
---

3点を有向線分に対する5種類の位置関係へ分類する。

## API

```cpp
int ccw(Point first, Point second, Point third);
```

`first` から `second` を基準とし、左、右、後方、前方、線分上を対応する
`CcwResult` の整数値で返す。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `ccw(first, second, third)` | $O(1)$ | $O(1)$ |

## 注意点

外積・内積は基準線分長と丸め誤差scaleを用いて判定する。
基準線分が許容誤差以下なら、同一点だけを `ON_SEGMENT` とする。
