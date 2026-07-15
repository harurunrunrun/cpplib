---
title: Solve 3x3 Linear System (三元連立一次方程式)
documentation_of: ../src/algorithm/geometry/3d/solve_linear_system_3x3.hpp
---

`Matrix3` と `Point3` で与えた三元連立一次方程式 $Ax=b$ を解く。

## API

- `solve_linear_system_3x3(matrix, rhs, relative_epsilon)`: 一意解を
  `Point3` で返す。数値的に特異なら `std::nullopt`。

## 時間計算量

時間・追加領域ともに $O(1)$。

## 注意点

- 部分pivotingと係数・右辺の独立scaleを用いる。
- 非有限値、不正な許容誤差には `std::invalid_argument`、表現不能な解には
  `std::overflow_error` を送出する。
