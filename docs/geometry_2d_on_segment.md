---
title: 2D Point on Segment (二次元線分上判定)
documentation_of: ../src/algorithm/geometry/2d/on_segment.hpp
---

## API

- `on_segment(segment, point)`: `point` が閉線分 `segment` 上にあるか判定する。

## API別の時間計算量・空間計算量

- `on_segment`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

端点を含み、退化線分は1点として扱う。線分方向に対する垂直距離と射影距離を `GEOMETRY_EPS` で判定する。
このため、生の外積・内積に絶対誤差を適用したときの線分長scale依存を持たない。
