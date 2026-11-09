---
title: Segment Intersection Record (線分交差記録)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/segment_intersection_record.hpp
---

交差する線分の対と、その交差の形状を保持する結果型。

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
- `kind`: 1 点での交差か、正の長さをもつ重なり区間かの種類。
- `first_point`, `second_point`: 1 点での交差では同じ交点。区間での交差では重なり区間の両端。

## 時間計算量

| 操作 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 構築、全フィールドの参照 | $O(1)$ | $O(1)$ |

## 注意点

`kind == SEGMENT_INTERSECTION_POINT` なら両点は同じ幾何的な点である。
`kind == SEGMENT_INTERSECTION_SEGMENT` なら両点はライブラリの標準の順序（`Point::operator<`）で並ぶ。

## 使用例

```cpp
const SegmentIntersectionRecord record{
    0, 1, SEGMENT_INTERSECTION_POINT, Point{1, 1}, Point{1, 1}
};
```
