---
title: 2D Right-Angle Rotation (二次元90度回転)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/point/rotate90.hpp
---

## API

- `Point rotate90(const Point& point)`: `point` を反時計回りに 90 度回転した `{-y, x}` を返す。

## API別の時間計算量・空間計算量

- `rotate90`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

回転の中心は原点である。浮動小数点の誤差は生じない。
