---
title: 2D Right-Angle Rotation (二次元90度回転)
documentation_of: ../src/algorithm/geometry/2d/point/rotate90.hpp
---

## API

- `rotate90(point)`: `point` を反時計回りへ90度回転した `{-y, x}` を返す。

## API別の時間計算量・空間計算量

- `rotate90`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

原点を回転中心とする。
