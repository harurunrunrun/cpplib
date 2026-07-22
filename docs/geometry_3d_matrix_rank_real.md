---
title: Real Matrix Rank (実数行列の階数)
documentation_of: ../src/algorithm/geometry/3d/scalar/matrix_rank_real.hpp
---

`long double` の一般矩形行列の数値rankを求める。

## API

- `matrix_rank_real(matrix, relative_epsilon)`: 部分pivoting付きGaussian
  eliminationで得たpivot数を返す。

## 時間計算量

$R\times C$ 行列に対し $O(RC\min(R,C))$ 時間、$O(RC)$ 追加領域。

## 注意点

- 入力全体の最大絶対値で正規化した後、`relative_epsilon` 以下のpivotを0とみなす。
- 行長が不一致、非有限値、許容誤差が不正なら `std::invalid_argument` を送出する。
