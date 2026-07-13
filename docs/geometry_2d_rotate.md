---
title: 2D Rotation (二次元回転)
documentation_of: ../src/algorithm/geometry/2d/rotate.hpp
---

## API

- `rotate(point, theta)`: 原点中心に `point` を反時計回りへ `theta` ラジアン回転する。

## API別の時間計算量・空間計算量

- `rotate`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

角度の単位はラジアンである。
