---
title: Solve 2x2 Linear System (二元連立一次方程式)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/scalar/solve_linear_system_2x2.hpp
---

二元連立一次方程式 $Ax = b$ を部分ピボット選択付きの消去法で解く。

## API

- `solve_linear_system_2x2(matrix, right_hand_side, relative_epsilon = GEOMETRY3D_EPS)`: 一意解 $(x, y)$ を `std::array<long double, 2>` で返す。数値的に特異なら `std::nullopt` を返す。

## 時間計算量・空間計算量

- 時間・追加領域ともに $O(1)$。

## 注意点

- 内部では `solve_square_linear_system_real` に委譲する。
- 係数と右辺を別々にスケーリングしてから解くため、非常に大きい一様なスケールも扱える。
- 非有限な値や不正な許容誤差（負または非有限）には `std::invalid_argument` を、表現できない解には `std::overflow_error` を送出する。
