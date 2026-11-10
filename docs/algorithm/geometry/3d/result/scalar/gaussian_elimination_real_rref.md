---
title: Real Gaussian Elimination to RREF (実数行列の簡約行階段形)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/scalar/gaussian_elimination_real_rref.hpp
---

実数行列をガウス・ジョルダン消去で簡約行階段形（RREF）に変形する。

## API

- `gaussian_elimination_real(matrix, relative_epsilon = GEOMETRY3D_EPS)`: 部分ピボット選択付きの消去を行い、簡約行階段形 `reduced_row_echelon_form` とピボット列の添字列 `pivot_columns` を持つ `RealGaussianEliminationResult` を返す。`matrix` は `std::vector<std::vector<long double>>` で与える。

## API別の時間計算量・空間計算量

$R \times C$ 行列に対して、時間 $O(RC \min(R, C))$、追加空間 $O(RC)$。

## 注意点

- 行列は矩形で全要素が有限、`relative_epsilon` は有限かつ非負でなければならない。違反した場合は `std::invalid_argument` を送出する。
- 計算前に全要素を最大絶対値で割って正規化し、正規化後のピボットの絶対値が `relative_epsilon` 以下ならその列は $0$ とみなして飛ばす。最後に絶対値が `relative_epsilon` 以下の要素を $0$ に丸める。
- 全要素が $0$ の行列では、入力をそのまま返し `pivot_columns` は空となる。
