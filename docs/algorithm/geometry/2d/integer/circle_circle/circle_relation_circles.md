---
title: Relation between Two Circles (2 円の位置関係)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/circle_circle/circle_relation_circles.hpp
---

2 円の位置関係を、包含・内接・2 点で交差・外接・分離・一致に分類する。

## API

```cpp
int circle_relation(const Circle& first, const Circle& second);
```

対応する列挙型 `CircleRelation` の値を整数で返す。

| 値 | 意味 |
| --- | --- |
| `CIRCLE_INSIDE` (0) | 一方が他方の内部にある |
| `CIRCLE_INTERNALLY_TANGENT` (1) | 内接する |
| `CIRCLE_INTERSECT` (2) | 2 点で交わる |
| `CIRCLE_EXTERNALLY_TANGENT` (3) | 外接する |
| `CIRCLE_SEPARATE` (4) | 離れている |
| `CIRCLE_COINCIDENT` (5) | 一致する |

引数の順序を入れ替えても分類は変わらない。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `circle_relation(first, second)` | $O(1)$ | $O(1)$ |

## 注意点

両円とも、座標が有限で、半径が有限かつ非負でなければならない。違反した場合は `std::invalid_argument` を送出する。
判定は中心間距離・半径の和・半径の差の大きさに比例する相対許容誤差で行う。同じ点を中心とする半径 0 の円同士は一致と判定する。
