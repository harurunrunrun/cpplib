---
title: Segment Intersection Record (線分交差記録)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/segment_intersection_record.hpp
---

交差する線分対と、その交差形状を保持する結果型。

## API

```cpp
struct SegmentIntersectionRecord {
    std::size_t first;
    std::size_t second;
    SegmentIntersectionKind kind;
    Point first_point;
    Point second_point;
};
```

- `first`, `second`: 入力線分の添字で、常に `first < second`。
- `kind`: 一点交差または正の長さを持つ重複区間の種類。
- `first_point`, `second_point`: 一点交差では同じ交点。区間交差では重複区間の両端。

## 時間計算量

| 操作 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 構築、全fieldの参照 | $O(1)$ | $O(1)$ |

## 注意点

`kind == SEGMENT_INTERSECTION_POINT` なら両点は同じ幾何点である。
`kind == SEGMENT_INTERSECTION_SEGMENT` なら両点はライブラリの標準順序で並ぶ。

## 使用例

```cpp
const SegmentIntersectionRecord record{
    0, 1, SEGMENT_INTERSECTION_POINT, Point{1, 1}, Point{1, 1}
};
```
