---
title: Counterclockwise Classification of Three Points (3 点の反時計回り分類)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/point_point_point/ccw_points.hpp
---

3 点の位置関係を、有向線分を基準に 5 種類に分類する。

## API

```cpp
int ccw(Point first, Point second, Point third);
```

`first` から `second` への有向線分を基準に、`third` の位置を対応する `CcwResult` の整数値で返す。

| 値 | 意味 |
| --- | --- |
| `COUNTER_CLOCKWISE` (1) | 左側 |
| `CLOCKWISE` (-1) | 右側 |
| `ONLINE_BACK` (2) | 直線上で `first` より後方 |
| `ONLINE_FRONT` (-2) | 直線上で `second` より前方 |
| `ON_SEGMENT` (0) | 線分上 |

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `ccw(first, second, third)` | $O(1)$ | $O(1)$ |

## 注意点

外積・内積の符号は、基準線分の長さと丸め誤差のスケールを用いた `geometry_scaled_sign` で判定する。
基準線分の長さが `GEOMETRY_EPS` 以下の場合は、`third` が `first` と（許容誤差の範囲で）一致すれば `ON_SEGMENT`、そうでなければ `ONLINE_FRONT` を返す。
