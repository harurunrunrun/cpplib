---
title: Real Gaussian Elimination (実数ガウス消去法)
documentation_of: ../src/algorithm/geometry/3d/query/gaussian_elimination_real.hpp
---

`long double` 行列に対し、絶対値最大の要素を pivot に選ぶ部分 pivoting 付き
Gaussian eliminationを行う。入力全体を最大絶対値で正規化してから計算するため、
一様な座標scaleの変更に対してrank判定が不変になる。

## API

- `gaussian_elimination_real(matrix, relative_epsilon)`: reduced row echelon form、
  pivot列を持つ `RealGaussianEliminationResult` を返す。
- `result.rank()`: pivot数、すなわち数値rankを返す。
- `solve_square_linear_system_real(matrix, rhs, relative_epsilon)`: 正方な
  $Ax=b$ の一意解を返す。数値的に特異なら `std::nullopt`。

## 時間計算量

$R\times C$ 行列の消去は $O(RC\min(R,C))$ 時間、$O(RC)$ 追加領域。
$N$ 元の求解は $O(N^3)$ 時間、$O(N^2)$ 追加領域。

## 注意点

- 行長が一致しない、非有限値を含む、許容誤差が不正な入力には
  `std::invalid_argument` を送出する。
- `relative_epsilon` 以下の正規化pivotを0とみなす。既定値は
  `GEOMETRY3D_EPS`。
- RREFは入力全体を正の定数で割った行列のものだが、元の行列のRREFと等しい。
- 表現不能な解には `std::overflow_error` を送出する。
