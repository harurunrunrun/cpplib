---
title: Real Matrix Determinant (実数行列の行列式)
documentation_of: ../src/algorithm/geometry/3d/scalar/matrix_determinant_real.hpp
---

`long double` の一般正方行列のdeterminantを部分pivoting付き消去法で求める。

## API

- `matrix_determinant_real(matrix, relative_epsilon)`: determinantを返す。
  $0\times0$ 行列の値は1、数値的に特異な行列の値は0。

## 時間計算量

$N\times N$ 行列に対し $O(N^3)$ 時間、$O(N^2)$ 追加領域。

## 注意点

- 正方でない、行長が不一致、非有限値、許容誤差が不正な入力には
  `std::invalid_argument` を送出する。
- 積は仮数と2進指数に分けて蓄積し、中間積の不要なoverflowを避ける。
  最終値が表現不能なら `std::overflow_error` を送出する。
