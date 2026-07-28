---
title: Relation between Two Circles (2円の位置関係)
documentation_of: ../../../../../../src/algorithm/geometry/2d/integer/circle_circle/circle_relation_circles.hpp
---

2円を包含、内接、2点交差、外接、分離、一致へ分類する。

## API

```cpp
int circle_relation(const Circle& first, const Circle& second);
```

対応する `CircleRelation` 列挙値を整数で返す。引数順を交換しても分類は変わらない。

## 時間計算量

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `circle_relation(first, second)` | $O(1)$ | $O(1)$ |

## 注意点

両円は有限座標と有限かつ非負の半径を持たなければならない。
違反時は `std::invalid_argument` を送出する。同一点の半径0円同士は一致となる。
