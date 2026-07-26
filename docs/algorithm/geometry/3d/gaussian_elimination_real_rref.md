---
title: Real Gaussian Elimination to RREF (実数行列の簡約行階段形)
documentation_of: ../../../../src/algorithm/geometry/3d/gaussian_elimination_real_rref.hpp
---

## API

- `gaussian_elimination_real(matrix, relative_epsilon)`: 部分pivoting付き消去を行い、簡約行階段形とpivot列を持つ `RealGaussianEliminationResult` を返す。

## API別の時間計算量・空間計算量

$R\times C$ 行列に対して時間 $O(RC\min(R,C))$、追加領域 $O(RC)$。

## 注意点

行列は矩形で全要素が有限、許容誤差は有限かつ非負でなければならない。最大絶対値で正規化し、正規化pivotが許容誤差以下なら0とみなす。
