---
title: 2D Segment Intersection Point (二次元線分交点)
documentation_of: ../src/algorithm/geometry/2d/segment_intersection.hpp
---

## API

- `segment_intersection(first, second)`: 2閉線分の一意な共通点を返す。

## API別の時間計算量・空間計算量

- `segment_intersection`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

共通点がない場合、または重なりが線分となり一意でない場合は `std::domain_error` を送出する。退化線分同士にも対応する。
