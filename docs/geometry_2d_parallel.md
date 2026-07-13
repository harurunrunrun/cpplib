---
title: 2D Parallelism (二次元平行判定)
documentation_of: ../src/algorithm/geometry/2d/parallel.hpp
---

## API

- `parallel(left, right)`: 2ベクトルが平行か判定する。
- `parallel(left_line, right_line)`: 2直線の方向ベクトルが平行か判定する。

## API別の時間計算量・空間計算量

- 各overload: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

許容誤差付きの外積判定を行う。退化直線は渡してはならない。
