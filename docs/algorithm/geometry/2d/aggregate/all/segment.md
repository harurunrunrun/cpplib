---
title: 2D Segment Aggregator (二次元線分集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/segment.hpp
---

線分型と、線分を含む直線（支持直線）への変換 API をまとめた後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `segment_type.hpp` | `Segment` |
| `supporting_line_segment.hpp` | `supporting_line(segment)` |

## 集約される API

```cpp
struct Segment {
    Point a;
    Point b;
};
Line supporting_line(const Segment& segment);
```

`Segment` は 2 つの端点を持つ閉線分を表す。`supporting_line` は両端点を通る無限直線 `Line{segment.a, segment.b}` を返す。

## 計算量

| API・操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `Segment` の構築・フィールドの参照 | $O(1)$ | $O(1)$ |
| `supporting_line(segment)` | $O(1)$ | $O(1)$ |

## 注意点

退化した線分を許すかどうかは利用する側の API による。`supporting_line` は退化した線分に対しても値を返すので、返された直線を使う API の前提は別に満たす必要がある。
