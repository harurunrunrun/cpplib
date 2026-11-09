---
title: 2D Counterclockwise Test Aggregator (二次元反時計回り判定集約ヘッダ) [CGL_1_C]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/ccw.hpp
---

位置関係の列挙値と、3 点を入力とする判定 API をまとめた後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `ccw_result.hpp` | `CcwResult` と 5 つの列挙値 |
| `ccw_points.hpp` | `ccw(first, second, third)` |

## 集約される API

```cpp
int ccw(Point first, Point second, Point third);
```

`first` から `second` への有向線分を基準として `third` の位置を判定し、`COUNTER_CLOCKWISE(1)`, `CLOCKWISE(-1)`, `ONLINE_BACK(2)`, `ONLINE_FRONT(-2)`, `ON_SEGMENT(0)` のいずれかを返す。

## 計算量

| API・操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `ccw`、列挙値の参照 | $O(1)$ | $O(1)$ |

## 注意点

- `first` と `second` が（許容誤差の範囲で）一致する退化した場合は、`third == first` なら `ON_SEGMENT`、それ以外なら `ONLINE_FRONT` を返す。
- 外積・内積の符号は、ベクトルの長さと丸め誤差の見積もりに応じた相対的な許容誤差で判定する。
