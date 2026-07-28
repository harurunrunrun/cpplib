---
title: Offline 3D Dominance Counting (オフライン三次元優越数え上げ)
documentation_of: ../../../../src/algorithm/range/offline_query/offline_3d_dominance.hpp
---

各点 $i$ について、$x_j\le x_i,y_j\le y_i,z_j\le z_i$ を満たす別の点 $j$ の個数を返す。

## API

- `DominancePoint3D`: 64-bit整数座標を保持する。
- `offline_3d_dominance(points)`: 入力順の答えを返す。

## 時間計算量

- $O(N\log^2 N)$ 時間、$O(N)$ 追加空間。

## 注意点

- 同一点が複数ある場合、それらは互いを数える。自分自身は数えない。
