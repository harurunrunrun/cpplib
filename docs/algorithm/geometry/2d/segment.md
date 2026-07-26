---
title: 2D Segment Aggregator (二次元線分集約ヘッダ)
documentation_of: ../../../../src/algorithm/geometry/2d/segment.hpp
---

線分型と支持直線変換APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `segment_type.hpp` | `Segment` |
| `supporting_line_segment.hpp` | `supporting_line(segment)` |

## 集約されるAPI

```cpp
struct Segment {
    Point a;
    Point b;
};
Line supporting_line(const Segment& segment);
```

`Segment` は2端点を持つ閉線分、`supporting_line` は両端点を通る無限直線を表す。

## API別の時間計算量・空間計算量

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Segmentの構築・field参照 | $O(1)$ | $O(1)$ |
| `supporting_line(segment)` | $O(1)$ | $O(1)$ |

## 注意点

退化線分を許すかは利用側APIによる。`supporting_line` は退化線分にも値を返すため、
返された直線を使うAPIの前提を別途満たす必要がある。
