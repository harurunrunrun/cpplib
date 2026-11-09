---
title: 2D Rotation (二次元回転)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/point/rotate.hpp
---

## API

- `Point rotate(const Point& point, long double theta)`: 原点を中心に、`point` を反時計回りに `theta` ラジアン回転した点を返す。

## API別の時間計算量・空間計算量

- `rotate`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

角度の単位はラジアンである。
