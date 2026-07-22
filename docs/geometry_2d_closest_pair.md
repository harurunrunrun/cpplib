---
title: 2D Closest Pair Distance (二次元最近点対距離) [CGL_5_A]
documentation_of: ../src/algorithm/geometry/2d/scalar/closest_pair.hpp
---

## API

- `closest_pair(points)`: 異なる2入力点のユークリッド距離の最小値を返す。

## API別の時間計算量・空間計算量

- 点数を $N$ として、時間 $O(N\log N)$、追加領域 $O(N)$。

## 注意点

2点未満の入力では正の無限大、重複点があれば0を返す。
