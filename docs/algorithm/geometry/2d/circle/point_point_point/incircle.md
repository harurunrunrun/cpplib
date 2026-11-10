---
title: 2D Incircle (二次元内接円) [CGL_7_B]
documentation_of: ../../../../../../src/algorithm/geometry/2d/circle/point_point_point/incircle.hpp
---

## API

- `Circle incircle(first, second, third)`: 三角形の内接円を返す。

## API別の時間計算量・空間計算量

- `incircle`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

中心は `incenter(first, second, third)` で求める。退化三角形には `incenter` が `std::invalid_argument` を送出する。
半径は、辺 `first`→`second` の単位方向ベクトルと、`first` から内心への差分との外積の絶対値として求め、辺ベクトル同士の積を避ける。
