---
title: Supporting Line of a Segment (線分の支持直線)
documentation_of: ../../../../src/algorithm/geometry/2d/supporting_line_segment.hpp
---

線分の両端点を通る無限直線を構築する。

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

退化線分にも退化直線を返し、この関数自体は例外を送出しない。
返された直線を利用するAPIの非退化前提は呼び出し側が満たす。
