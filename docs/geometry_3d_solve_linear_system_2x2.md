---
title: Solve 2x2 Linear System (二元連立一次方程式)
documentation_of: ../src/algorithm/geometry/3d/solve_linear_system_2x2.hpp
---

二元連立一次方程式 $Ax=b$ を部分pivoting付き消去法で解く。

## API

- `solve_linear_system_2x2(matrix, rhs, relative_epsilon)`: 一意解
  `(x, y)` を返す。数値的に特異なら `std::nullopt`。

## 時間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 係数と右辺を別々にscaleしてから解くため、非常に大きい一様scaleを扱える。
- 非有限値、不正な許容誤差には `std::invalid_argument`、表現不能な解には
  `std::overflow_error` を送出する。
