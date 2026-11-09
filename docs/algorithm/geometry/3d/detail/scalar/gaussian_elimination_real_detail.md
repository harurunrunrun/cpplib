---
title: Real Linear Algebra Internal Detail (実数線形代数内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/3d/detail/scalar/gaussian_elimination_real_detail.hpp
---

実数行列の検証、許容誤差の検証、スケールの計算を 2 つの公開個別ヘッダで共有する。

## API

名前空間 `geometry3d_linear_algebra_detail` に次の関数を置く。

- `validate_real_matrix(matrix)`: 各行の長さが等しいこと（矩形であること）と全要素が有限であることを検査し、`{行数, 列数}` を返す。違反時は `std::invalid_argument` を送出する。
- `validate_relative_epsilon(epsilon)`: 許容誤差が有限かつ非負であることを検査する。違反時は `std::invalid_argument` を送出する。
- `maximum_absolute_entry(matrix)`: 要素の絶対値の最大値を返す。

## API別の時間計算量・空間計算量

$R\times C$ 行列について、`validate_real_matrix` と `maximum_absolute_entry` は時間 $O(RC)$、追加領域 $O(1)$。`validate_relative_epsilon` は $O(1)$。

## 注意点

内部実装用のヘッダであり、互換性を保証しない。公開 API としては、RREF または正方連立一次方程式ソルバの個別ヘッダを使うこと。
