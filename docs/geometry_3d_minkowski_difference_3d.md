---
title: Minkowski Difference 3D (3次元Minkowski差)
documentation_of: ../src/algorithm/geometry/3d/minkowski_difference_3d.hpp
---

## API

- `minkowski_difference_3d(first, second)`: 全ての $a-b$ の凸包を返す。原点包含は2形状の交差判定に利用できる。

## API別の時間計算量・空間計算量

- $P=V_1V_2$、構築中の最大面数を $F$ として時間 $O(PF)$、領域 $O(P+F)$。

## 注意点

両入力は空でない凸形状でなければならず、空入力は `std::invalid_argument`。
