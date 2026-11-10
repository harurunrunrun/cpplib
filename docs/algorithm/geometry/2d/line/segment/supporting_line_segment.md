---
title: Supporting Line of a Segment (線分の支持直線)
documentation_of: ../../../../../../src/algorithm/geometry/2d/line/segment/supporting_line_segment.hpp
---

線分の両端点を通る無限直線を作る。

## API

```cpp
Line supporting_line(const Segment& segment);
```

`Line{segment.a, segment.b}` と同じ値を返す。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `supporting_line(segment)` | $O(1)$ | $O(1)$ |

## 注意点

退化した線分には退化した直線を返し、この関数自体は例外を送出しない。
返された直線を使う API が非退化を前提とする場合、その前提は呼び出し側で満たす必要がある。
