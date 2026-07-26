---
title: Real Square Linear System Solver (実数正方連立一次方程式)
documentation_of: ../../../../src/algorithm/geometry/3d/solve_square_linear_system_real.hpp
---

## API

- `solve_square_linear_system_real(matrix, right_hand_side, relative_epsilon)`: $Ax=b$ の一意解を返す。数値的に特異なら `std::nullopt`。

## API別の時間計算量・空間計算量

$N$ 元に対して時間 $O(N^3)$、追加領域 $O(N^2)$。

## 注意点

行列は $N\times N$ で右辺長も $N$、全要素は有限、許容誤差は有限かつ非負でなければならない。表現不能な解には `std::overflow_error` を送出する。
