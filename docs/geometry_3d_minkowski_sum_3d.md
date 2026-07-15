---
title: Minkowski Sum 3D (3次元Minkowski和)
documentation_of: ../src/algorithm/geometry/3d/minkowski_sum_3d.hpp
---

## API

- `minkowski_sum_3d(first, second)`: 全ての $a+b$ の凸包を `ConvexPolyhedron3` として返す。

## API別の時間計算量・空間計算量

- 頂点数を $V_1,V_2$、候補数を $P=V_1V_2$、構築中の最大面数を $F$ として時間 $O(PF)$、領域 $O(P+F)$。

## 注意点

両入力は空でない凸形状でなければならず、空入力は `std::invalid_argument`。重複点と退化次元は凸包側で処理する。
