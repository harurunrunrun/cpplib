---
title: 2D Counterclockwise Test Aggregator (二次元反時計回り判定集約ヘッダ) [CGL_1_C]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/ccw.hpp
---

位置関係の列挙値と3点入力の判定APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `ccw_result.hpp` | `CcwResult` と5列挙値 |
| `ccw_points.hpp` | `ccw(first, second, third)` |

## 集約されるAPI

```cpp
int ccw(Point first, Point second, Point third);
```

返り値は `COUNTER_CLOCKWISE(1)`, `CLOCKWISE(-1)`, `ONLINE_BACK(2)`,
`ONLINE_FRONT(-2)`, `ON_SEGMENT(0)` のいずれかである。

## API別の時間計算量・空間計算量

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `ccw`、列挙値参照 | $O(1)$ | $O(1)$ |

## 注意点

`first` から `second` への有向線分を基準とする。退化基準では `third == first` のとき
`ON_SEGMENT`、それ以外は `ONLINE_FRONT` を返す。
