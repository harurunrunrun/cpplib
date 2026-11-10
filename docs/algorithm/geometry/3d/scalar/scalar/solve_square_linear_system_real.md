---
title: Real Square Linear System Solver (実数正方連立一次方程式)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/scalar/solve_square_linear_system_real.hpp
---

## API

- `solve_square_linear_system_real(matrix, right_hand_side, relative_epsilon = GEOMETRY3D_EPS)`: $Ax = b$ の一意解を `std::vector<long double>` で返す。数値的に特異なら `std::nullopt` を返す。

## API別の時間計算量・空間計算量

- $N$ 元に対して、時間 $O(N^3)$、追加領域 $O(N^2)$。

## 注意点

- 係数行列を最大絶対値の要素で、右辺を最大絶対値でそれぞれ正規化してから部分ピボット選択付きの消去法で解く。ピボットの絶対値が `relative_epsilon` 以下なら特異とみなす。係数がすべて $0$ の場合も `std::nullopt` を返す。$N = 0$ なら空の列を返す。
- 行列は $N \times N$、右辺の長さは $N$、全要素は有限、許容誤差は有限かつ非負でなければならない。違反時は `std::invalid_argument` を送出する。
- 表現できない解には `std::overflow_error` を送出する。
